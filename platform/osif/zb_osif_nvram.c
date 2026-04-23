/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2026 NXP
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: File i/o trace, traffic dump and, maybe, other unix-specific
*/

#define ZB_TRACE_FILE_ID 34045
#include "zboss_api_core.h"
#include "zboss_api.h"
#include "zboss_api_error.h"
#include <errno.h>


#undef DEBUG_NVRAM
#undef DEBUG_NVS
#undef DEBUG_DATA

#ifdef DEBUG_DATA
#define DEBUG_PRINT_BUFFER(buffer, len, format, ...) wcs_print_buf((const zb_uint8_t *)buffer, len, format, ##__VA_ARGS__)
#else
#define DEBUG_PRINT_BUFFER(...) do {} while(0)
#endif

#define ZB_NVRAM_PAGE_SIZE   0x4000
#define ZB_NVRAM_PAGE_COUNT  2


#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/kvss/nvs.h>

#define ZB_NVRAM_ID       FIXED_PARTITION_ID(zb_nvram)
#define ZB_NVRAM_DEV      FIXED_PARTITION_DEVICE(zb_nvram)
#define ZB_NVRAM_OFF      FIXED_PARTITION_OFFSET(zb_nvram)


/*! \addtogroup ZB_OSIF */
/*! @{ */

#if defined ZB_USE_NVRAM || defined DOXYGEN

void zb_error_raise(zb_uint8_t severity, zb_ret_t err_code, void *additional_info);
void zb_nvram_erase_finished(zb_uint8_t page);

typedef ZB_PACKED_PRE struct zb_nvram_dataset_hdr_s {
  zb_uint32_t time_label;        /*!< Time index - not real or ZigBee time */
  zb_uint16_t data_len;          /*!< Record length. It equal header length + dataset length  */
  zb_uint16_t data_set_type;     /*!< Dataset type. See @par zb_nvram_dataset_types_t  */
  zb_uint16_t data_set_version;  /*!< Dataset version. Unique per each dataset  */
  zb_uint8_t transaction_status; /*!< Current transaction status. @see @ref nvram_transaction_state  */
  zb_uint8_t reserved;           /*!< Alignment. Reserved for future use  */
} ZB_PACKED_STRUCT zb_nvram_dataset_hdr_t;

typedef ZB_PACKED_PRE struct zb_nvram_dataset_tail_s {
  zb_uint32_t   time_label;     /*!< Time index - not real or ZigBee time - kind of TSN */
  zb_uint16_t   crc;            /*!< CRC16 field for validate dataset integrity */
  zb_uint16_t   reserved;       /*!< Reserved field for future needs */
} ZB_PACKED_STRUCT zb_nvram_dataset_tail_t;


typedef ZB_PACKED_PRE struct zb_nvram_dataset_info_s {
  zb_uint32_t pos;
  zb_uint16_t len;
  zb_uint16_t ver;
  zb_uint8_t  page;
  zb_uint8_t  reserved;
} ZB_PACKED_STRUCT zb_nvram_dataset_info_t;

/* Last 2 indexes will store ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR of each page */
/* index is used for nvsId, datasetInfo is stored in MAX_DATASETS nvsId */
#define MAX_DATASETS              ZB_NVRAM_DATASET_NUMBER+2
zb_nvram_dataset_info_t datasetInfo[MAX_DATASETS] = {0};


typedef struct {
  struct nvs_fs fs;
  zb_uint8_t    id;
} zephyr_flash_t;

static zephyr_flash_t gc_flash_nvram = {
  .fs   = {
    .flash_device = ZB_NVRAM_DEV,
    .offset       = ZB_NVRAM_OFF,
  },
  .id    = ZB_NVRAM_ID
};

static zb_bool_t nvram_inited = ZB_FALSE;

/* Variables for local storage of the current dataset */
static zb_uint16_t  localNvsId    = 0;
#define LOCAL_BUF_ALLOC_SIZE 256 /* To consider to increase it for a ZCZR to avoid re-allocating it at runtime */
static zb_uint8_t  *localNvsBuf   = NULL;
static size_t       localNvsSize  = 0;
static zb_bool_t    localNvsFlush = ZB_FALSE;

/* Variables for the relocation of dataset from old page.pos to new page.pos */
zb_uint16_t             relocateDataset = (zb_uint16_t)-1;
zb_nvram_dataset_info_t relocateOldInfo = {0};
zb_uint32_t             relocateOffset  = 0;
zb_uint16_t             relocateLen     = 0;


static int zb_osif_nvram_open(zephyr_flash_t *flash)
{
  int ret = 0;
  struct flash_pages_info info;
  zb_uint32_t   align;
  ssize_t free;
  size_t max;

  if(!device_is_ready(flash->fs.flash_device)) {
    WCS_TRACE_ERROR("%s() ERROR: flash area zb_nvram not ready", __FUNCTION__);
    return -1;
  }

  ret =  flash_get_page_info_by_offs(flash->fs.flash_device, flash->fs.offset, &info);
  if(ret != 0) {
    WCS_TRACE_ERROR("%s() ERROR: flash area zb_nvram get info failed", __FUNCTION__);
    return -1;
  }
  flash->fs.sector_size = info.size;
  flash->fs.sector_count = ZB_NVRAM_PAGE_SIZE/info.size;

  ret = nvs_mount(&flash->fs);
  if(ret != 0) {
    WCS_TRACE_ERROR("%s() ERROR: flash area zb_nvram mount failed", __FUNCTION__);
    return -1;
  }

  align = flash->fs.flash_parameters->write_block_size;
  free = nvs_calc_free_space(&flash->fs);
  max = nvs_sector_max_data_size(&flash->fs);
  WCS_TRACE_INFO("Flash  %d sectors size 0x%x, write_align 0x%x, free space 0x%x, max data size 0x%x", 
    flash->fs.sector_count, flash->fs.sector_size, align, free, max);

  return 0;
}

void zb_osif_nvram_init(const zb_char_t *name)
{
  int ret;
  zb_verify_additional_info_t add_info = {
    .file_id                = ZB_TRACE_FILE_ID,
    .caller_additional_info = NULL,
  };

  ZVUNUSED(name);

  if (nvram_inited)
    return;

  ret = zb_osif_nvram_open(&gc_flash_nvram);
  if(ret != 0) {
    add_info.line_number = __LINE__;
    zb_error_raise(ZB_ERROR_SEVERITY_FATAL, RET_NO_RESOURCES, &add_info);
  }

  /* Read table mapping beween position & nvs_id */
  ret = nvs_read(&gc_flash_nvram.fs, MAX_DATASETS, datasetInfo, sizeof(datasetInfo));
  if(ret != sizeof(datasetInfo)) {
    zb_uint8_t p;

#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("READ <<datasetInfo>> err %d nvs_delete(--, %d)", ret, MAX_DATASETS);
#endif
    nvs_delete(&gc_flash_nvram.fs, MAX_DATASETS);
    for(p=0;p<ZB_NVRAM_PAGE_COUNT;p++)
    {
      zb_osif_nvram_erase_async(p);
    }
  }
  else
  {
    zb_uint32_t next_pos = datasetInfo[ZB_NVRAM_DATASET_NUMBER].pos + datasetInfo[ZB_NVRAM_DATASET_NUMBER].len;

#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("READ <<datasetInfo>> nvs_read(--, %d, --, %d) ret %d", MAX_DATASETS, sizeof(datasetInfo), ret);
    //if(ret == sizeof(datasetInfo))
    //  DEBUG_PRINT_BUFFER(datasetInfo, sizeof(datasetInfo), "%s-nvs_read(--, %d, --, %d)", __FUNCTION__, MAX_DATASETS, sizeof(datasetInfo));
#endif

    /* Fake all datasets on continous positions in page 0 (starting by PAGE_HDR on each page) */
    for(zb_uint16_t idx = 0; idx < ZB_NVRAM_DATASET_NUMBER; idx++) {
      if(datasetInfo[idx].len == 0)
        continue;

#ifdef DEBUG_DATA
      WCS_TRACE_DEBUG("READ <<dataset>> [%02d]: page %d, pos %d moveTo %d, len %d => %s",
        idx,
        datasetInfo[idx].page,
        datasetInfo[idx].pos, next_pos,
        datasetInfo[idx].len,
        get_nvram_dataset_str((idx < ZB_NVRAM_DATASET_NUMBER)?(idx):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)));
#endif

      datasetInfo[idx].pos = next_pos;
      datasetInfo[idx].page = 0;
      next_pos += datasetInfo[idx].len;

      ZB_ASSERT(next_pos < ZB_NVRAM_PAGE_SIZE);
    }
  }

#ifdef ZB_PRODUCTION_CONFIG
  zb_osif_prod_cfg_init();
#endif

  localNvsSize += LOCAL_BUF_ALLOC_SIZE;
  localNvsBuf = ZB_MALLOC(localNvsSize);

  nvram_inited = ZB_TRUE;
}

void zb_osif_nvram_deinit(void)
{
  if(nvram_inited) {
    zb_osif_nvram_flush();
#ifdef ZB_PRODUCTION_CONFIG
    zb_osif_prod_cfg_deinit();
#endif

    localNvsSize = 0;
    ZB_FREE(localNvsBuf);

    nvram_inited = ZB_FALSE;
  }
}

zb_uint32_t zb_get_nvram_page_length()
{
  return ZB_NVRAM_PAGE_SIZE;
}

zb_uint8_t zb_get_nvram_page_count()
{
  return ZB_NVRAM_PAGE_COUNT;
}


static zb_uint16_t build_dataset_info(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len)
{
  zb_uint16_t oldLen = 0;
  zb_nvram_dataset_hdr_t *dataset = (zb_nvram_dataset_hdr_t *)buf;

  /*if(pos == 0) WCS_TRACE_DEBUG("%s(%d, %d, --, %d) search dataset %s: page %d, position %d, len %d, version %d", __FUNCTION__,
          page, pos, len,
          get_nvram_dataset_str(dataset->data_set_type),
          page,
          pos,
          dataset->data_len,
          dataset->data_set_version);*/

  if(len == sizeof(zb_nvram_dataset_hdr_t) &&
     dataset->data_len != 0xFFFF &&
     /*dataset->data_set_version != 0xFFFF &&*/
     (dataset->data_set_type < ZB_NVRAM_DATASET_NUMBER || dataset->data_set_type == ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)) 
  {
    /* This can be a dataset header */
    zb_uint16_t idx = (dataset->data_set_type == ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR) ? (ZB_NVRAM_DATASET_NUMBER+page):(dataset->data_set_type);
    zb_bool_t infoUpdated = ZB_FALSE;

    if(datasetInfo[idx].pos == 0) {
#ifdef DEBUG_NVS
        WCS_TRACE_DEBUG("%s(%d, %d, --, %d) register dataset %s: page %d, position %d, len %d, version %d", __FUNCTION__,
          page, pos, len,
          get_nvram_dataset_str(dataset->data_set_type),
          page,
          pos,
          dataset->data_len,
          dataset->data_set_version);
#endif
        infoUpdated = ZB_TRUE;
    } else {
      if(datasetInfo[idx].page != page) {
#ifdef DEBUG_NVS
        WCS_TRACE_DEBUG("%s(%d, %d, --, %d) dataset %s page has changed %d -> %d", __FUNCTION__,
          page, pos, len,
          get_nvram_dataset_str(dataset->data_set_type),
          datasetInfo[idx].page,
          page);
#endif
        infoUpdated = ZB_TRUE;
        relocateDataset = idx;
      }
      if(datasetInfo[idx].pos != pos) {
#ifdef DEBUG_NVS
        WCS_TRACE_DEBUG("%s(%d, %d, --, %d) dataset %s position has changed %d -> %d", __FUNCTION__,
          page, pos, len,
          get_nvram_dataset_str(dataset->data_set_type),
          datasetInfo[idx].pos,
          pos);
#endif
        infoUpdated = ZB_TRUE;
        relocateDataset = idx;
      }
      if(datasetInfo[idx].len != dataset->data_len) {
#ifdef DEBUG_NVS
        WCS_TRACE_DEBUG("%s(%d, %d, --, %d) dataset %s len has changed %d -> %d", __FUNCTION__,
          page, pos, len,
          get_nvram_dataset_str(dataset->data_set_type),
          datasetInfo[idx].len,
          dataset->data_len);
#endif
        oldLen = datasetInfo[idx].len;
        infoUpdated = ZB_TRUE;
        relocateDataset = idx;
      }
      if(datasetInfo[idx].ver != dataset->data_set_version) {
#ifdef DEBUG_NVS
        WCS_TRACE_DEBUG("%s(%d, %d, --, %d) dataset %s version has changed %d -> %d", __FUNCTION__,
          page, pos, len,
          get_nvram_dataset_str(dataset->data_set_type),
          datasetInfo[idx].ver,
          dataset->data_set_version);
#endif
        infoUpdated = ZB_TRUE;
      }
    }

    if(infoUpdated)
    {
      int ret;

      if(relocateDataset != (zb_uint16_t)-1)
        ZB_MEMCPY(&relocateOldInfo, &datasetInfo[idx], sizeof(relocateOldInfo));
      datasetInfo[idx].page = page;
      datasetInfo[idx].pos  = pos;
      datasetInfo[idx].ver  = dataset->data_set_version;
      datasetInfo[idx].len  = dataset->data_len;

      ret = nvs_write(&gc_flash_nvram.fs, MAX_DATASETS, datasetInfo, sizeof(datasetInfo));
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("WRITE <<datasetInfo>> nvs_write(--, %d, --, %d) ret %d", MAX_DATASETS, sizeof(datasetInfo), ret);
//    DEBUG_PRINT_BUFFER(datasetInfo, sizeof(datasetInfo), "%s-nvs_write(--, %d, --, %d)", __FUNCTION__, MAX_DATASETS, sizeof(datasetInfo));
#endif
    }
  }
  
  return oldLen;
}

static zb_uint16_t find_dataset(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t len)
{
  for(zb_uint16_t idx=0; idx < MAX_DATASETS; idx++) {
    if(page != datasetInfo[idx].page)
      continue;

    if(pos >= datasetInfo[idx].pos && pos < datasetInfo[idx].pos + datasetInfo[idx].len)
      return idx;
  }

  return (zb_uint16_t)-1;
}

static int do_localNvs_write(void)
{
  int ret = 0;

  if(localNvsFlush) {
    ret = nvs_write(&gc_flash_nvram.fs, localNvsId, localNvsBuf, datasetInfo[localNvsId].len);
    if(ret < datasetInfo[localNvsId].len && ret >= 0)
    {
#ifdef DEBUG_NVS
    WCS_TRACE_ERROR("dataset %s nvs_write(--, %d, --, %d) ret %d, should be %d, DELETE IT!",
      get_nvram_dataset_str((localNvsId < ZB_NVRAM_DATASET_NUMBER)?(localNvsId):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)),
      localNvsId, datasetInfo[localNvsId].len, ret, datasetInfo[localNvsId].len);
#endif
      nvs_delete(&gc_flash_nvram.fs, localNvsId);
      ret = nvs_write(&gc_flash_nvram.fs, localNvsId, localNvsBuf, datasetInfo[localNvsId].len);
    }
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("dataset %s nvs_write(--, %d, --, %d) ret %d",
      get_nvram_dataset_str((localNvsId < ZB_NVRAM_DATASET_NUMBER)?(localNvsId):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)),
      localNvsId, datasetInfo[localNvsId].len, ret);
    DEBUG_PRINT_BUFFER(localNvsBuf, datasetInfo[localNvsId].len, "%s-nvs_write(--, %d, --, %d)", __FUNCTION__, localNvsId, datasetInfo[localNvsId].len);
#endif

    localNvsFlush = ZB_FALSE;
  }

  return ret;
}

static int do_localNvs_read(zb_uint16_t nvsId, zb_uint16_t oldLen)
{
  int ret = 0;
  zb_uint16_t readLen = (oldLen)?(oldLen):(datasetInfo[nvsId].len);

  if(localNvsId != nvsId) {
    /* In case we change the dataset before doing the write tail (that trigs the write) */
    do_localNvs_write();

    localNvsId = nvsId;
    if(localNvsSize < datasetInfo[localNvsId].len) {
      WCS_TRACE_ERROR("Re-allocate localNvsBuf len %d -> %d, current dataset needs %d", localNvsSize, localNvsSize + LOCAL_BUF_ALLOC_SIZE, datasetInfo[localNvsId].len);
      /* When allocating at runtime (not at init), we faced issue "heap corruption" elsewhere...  */
      localNvsSize += LOCAL_BUF_ALLOC_SIZE;
      localNvsBuf = ZB_REALLOC(localNvsBuf, localNvsSize);
    }

    ret = nvs_read(&gc_flash_nvram.fs, localNvsId, localNvsBuf, readLen);
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("dataset %s nvs_read(--, %d, --, %d) ret %d",
      get_nvram_dataset_str((localNvsId < ZB_NVRAM_DATASET_NUMBER)?(localNvsId):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)),
      localNvsId, readLen, ret);
    DEBUG_PRINT_BUFFER(localNvsBuf, readLen, "%s-nvs_read(--, %d, --, %d)", __FUNCTION__, localNvsId, readLen);
#endif
    if(ret < readLen) {
        if(ret < 0) ret = 0;
        /* Not found, fake an empty data */
        ZB_MEMSET(localNvsBuf/*+ret*/, 0xFF, datasetInfo[localNvsId].len/* - ret*/);
    }
    if(readLen != datasetInfo[localNvsId].len) {
      /* Dataset len will change, delete it */
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("dataset %s len changed, nvs_delete(--, %d) ret %d",
      get_nvram_dataset_str((localNvsId < ZB_NVRAM_DATASET_NUMBER)?(localNvsId):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)),
      localNvsId, ret);
#endif
      nvs_delete(&gc_flash_nvram.fs, nvsId);
    }

    localNvsFlush = ZB_FALSE;
  }

  return ret;
}

#ifdef ZB_NXP_WCS_TRACE
static char *get_dataset_section_str(zb_uint16_t idx, zb_uint32_t offset, zb_uint16_t len)
{
  static char str[64];
  zb_uint16_t data_set_type;
  zb_uint16_t dataLen;

  if(idx == (zb_uint16_t)-1) {
    return "NOT FOUND";
  }

  data_set_type = (idx < ZB_NVRAM_DATASET_NUMBER)?(idx):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR);
  dataLen = datasetInfo[idx].len - sizeof(zb_nvram_dataset_hdr_t) - sizeof(zb_nvram_dataset_tail_t);

  if(offset < sizeof(zb_nvram_dataset_hdr_t))
  { /* HEADER */
    if(len > sizeof(zb_nvram_dataset_hdr_t))
      sprintf(str, "%s - header & more, dataLen %d", get_nvram_dataset_str(data_set_type), dataLen);
    else
      sprintf(str, "%s - header, dataLen %d", get_nvram_dataset_str(data_set_type), dataLen);
  }
  else if(offset < datasetInfo[idx].len - sizeof(zb_nvram_dataset_tail_t))
  { /* BODY */
    sprintf(str, "%s - body%s", get_nvram_dataset_str(data_set_type), (len < dataLen)?(" (partial)"):(""));
  }
  else if(offset < datasetInfo[idx].len)
  { /* TAIL */
    sprintf(str, "%s - tail%s", get_nvram_dataset_str(data_set_type), (len < sizeof(zb_nvram_dataset_tail_t))?(" (partial)"):(""));
  }
  else
  { /* OUT OF RANGE, should not happend  */
    sprintf(str, "%s - OUT OF RANGE", get_nvram_dataset_str(data_set_type));
  }

  return str;
}
#endif

zb_ret_t zb_osif_nvram_read(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len )
{
  int zRet = RET_OK;
  zb_uint16_t nvsId;
  zb_uint32_t offset;
  zb_bool_t relocate = ZB_FALSE;

  if(!device_is_ready(gc_flash_nvram.fs.flash_device)) {
    WCS_TRACE_ERROR("%s() ERROR: flash area zb_nvram not ready", __FUNCTION__);
    return RET_DEVICE_NOT_FOUND;
  }

  if(page>=ZB_NVRAM_PAGE_COUNT)
  {
    return RET_PAGE_NOT_FOUND;
  }
  if(pos+len>ZB_NVRAM_PAGE_SIZE || len == 0)
  {
    return RET_INVALID_PARAMETER;
  }

  nvsId = find_dataset(page, pos, len);
  if(nvsId == (zb_uint16_t)-1) {
    if(relocateDataset != (zb_uint16_t)-1 &&
       page == relocateOldInfo.page       &&
       pos >= relocateOldInfo.pos         &&
       pos + len < relocateOldInfo.pos + relocateOldInfo.len) {
      /* It reads the old location to next write the new location */
      nvsId = relocateDataset;
      relocateOffset = pos - relocateOldInfo.pos;
      relocateLen    = len;
      offset = relocateOffset;
      relocate = ZB_TRUE;
    } else {
      /* Not found, fake an empty data */
      ZB_MEMSET(buf, 0xFF, len);
      return RET_OK;
    }
  } else {
    relocateDataset = (zb_uint16_t)-1;
    relocateOffset  = 0;
    relocateLen     = 0;
    offset = pos - datasetInfo[nvsId].pos;
  }

  do_localNvs_read(nvsId, 0);

  ZB_MEMCPY(buf, localNvsBuf+offset, len);

#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s(%d, %d, --, %d) %s => %s%s",
    __FUNCTION__, page, pos, len, wcs_get_error_str(zRet), (relocate)?("Relocate "):(""), get_dataset_section_str(nvsId, offset, len));
  DEBUG_PRINT_BUFFER(buf, len, "%s(%d, %d, --, %d)", __FUNCTION__, page, pos, len);
#endif

  return zRet;
}

zb_ret_t zb_osif_nvram_write(zb_uint8_t page, zb_uint32_t pos, void *buf, zb_uint16_t len )
{
  int iRet = 0;
  int zRet = RET_OK;
  zb_uint16_t oldLen;
  zb_uint16_t nvsId;
  zb_uint32_t offset;
  zb_bool_t relocate = ZB_FALSE;

  if(!device_is_ready(gc_flash_nvram.fs.flash_device)) {
    WCS_TRACE_ERROR("%s() ERROR: flash area zb_nvram not ready", __FUNCTION__);
    return RET_DEVICE_NOT_FOUND;
  }

  if(page>=ZB_NVRAM_PAGE_COUNT)
  {
    return RET_PAGE_NOT_FOUND;
  }
  if(pos+len>ZB_NVRAM_PAGE_SIZE || len == 0)
  {
    return RET_INVALID_PARAMETER;
  }

  oldLen = build_dataset_info(page, pos, buf, len);
  nvsId = find_dataset(page, pos, len);
  if(nvsId == (zb_uint16_t)-1) {
    WCS_TRACE_ERROR("%s() ERROR: corresponding dataset not found", __FUNCTION__);
    return RET_ILLEGAL_REQUEST;
  }

  do_localNvs_read(nvsId, oldLen);

  offset = pos - datasetInfo[nvsId].pos;

  if(relocateDataset       == nvsId  &&
     relocateOffset == offset &&
     relocateLen    == len) {
      /* Now it writes to the new location when was previously read from the old location */
      if(ZB_MEMCMP(localNvsBuf+offset, buf, len)) {
        DEBUG_PRINT_BUFFER(buf, len, "%s(%d, %d, --, %d)", __FUNCTION__, page, pos, len);
        WCS_TRACE_ERROR("%s(%d, %d, --, %d) ERROR, this is not a relocation  => %s, data is different",
          __FUNCTION__, page, pos, len, get_dataset_section_str(nvsId, offset, len));
        return RET_ILLEGAL_REQUEST;
      }
      relocate = ZB_TRUE;
  } else {
    ZB_MEMCPY(localNvsBuf+offset, buf, len);
    localNvsFlush = ZB_TRUE;
  }

  /* Do the write only when writing the tail (last step) */
  if(offset == datasetInfo[nvsId].len - sizeof(zb_nvram_dataset_tail_t) && len == sizeof(zb_nvram_dataset_tail_t))
  {
    iRet = do_localNvs_write();
    if(iRet < datasetInfo[localNvsId].len)
      zRet = RET_OPERATION_FAILED;
  }

#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s(%d, %d, --, %d) %s => %s%s",
    __FUNCTION__, page, pos, len, wcs_get_error_str(zRet), (relocate)?("Relocate "):(""), get_dataset_section_str(nvsId, offset, len));
  DEBUG_PRINT_BUFFER(buf, len, "%s(%d, %d, --, %d)", __FUNCTION__, page, pos, len);
#endif

  return zRet;
}

#warning assume it is a single complete dataset
zb_ret_t zb_osif_nvram_move(zb_uint8_t src_page, zb_uint32_t src_pos, zb_uint8_t dst_page, zb_uint32_t dst_pos, void *buf, zb_uint16_t len)
{
  zb_ret_t status;

  status = zb_osif_nvram_read(src_page, src_pos, buf, len);
  if (status == RET_OK)
  {
    build_dataset_info(dst_page, dst_pos, buf, len);
  }

  return status;
}


zb_ret_t zb_osif_nvram_erase_async(zb_uint8_t page)
{
  int i;

  if(!device_is_ready(gc_flash_nvram.fs.flash_device)) {
    WCS_TRACE_ERROR("%s(%d) ERROR: flash area zb_nvram not ready", __FUNCTION__, page);
    return RET_DEVICE_NOT_FOUND;
  }

  if(page>=ZB_NVRAM_PAGE_COUNT)
  {
    return RET_PAGE_NOT_FOUND;
  }

  /* Clear each nvs entry on that page */
  for(i=0; i<ZB_NVRAM_DATASET_NUMBER;i++) {
    if(datasetInfo[i].page != page)
      continue;
    nvs_delete(&gc_flash_nvram.fs, i);
    memset(&datasetInfo[i], 0, sizeof(datasetInfo[i]));
  }

  /* Clear ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR of that page */
  i = ZB_NVRAM_DATASET_NUMBER+page;
  nvs_delete(&gc_flash_nvram.fs, i);
  memset(&datasetInfo[i], 0, sizeof(datasetInfo[i]));

  i = nvs_write(&gc_flash_nvram.fs, MAX_DATASETS, datasetInfo, sizeof(datasetInfo));
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("ERASE <<datasetInfo>> nvs_write(--, %d, --, %d) ret %d", MAX_DATASETS, sizeof(datasetInfo), i);
    DEBUG_PRINT_BUFFER(datasetInfo, sizeof(datasetInfo), "%s-nvs_write(--, %d, --, %d)", __FUNCTION__, MAX_DATASETS, sizeof(datasetInfo));
#endif

  /* Clear localNvsBuf */
  if(localNvsBuf)
    ZB_MEMSET(localNvsBuf, 0, localNvsSize);
  localNvsId = 0;

  /* Really, erase is synchronous for file based nvram */
  zb_nvram_erase_finished(page);

#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s(%d) %s (from %d to %d)", __FUNCTION__, page, wcs_get_error_str(RET_OK), page*ZB_NVRAM_PAGE_SIZE, (page+1)*ZB_NVRAM_PAGE_SIZE - 1);
#endif

  return RET_OK;
}

void zb_osif_nvram_flush()
{
#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s()", __FUNCTION__);
#endif
}
#endif
/*! @} */

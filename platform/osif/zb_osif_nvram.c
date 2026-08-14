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

#define ZB_NVRAM_DEV      PARTITION_DEVICE(zb_nvram)
#define ZB_NVRAM_OFF      PARTITION_OFFSET(zb_nvram)


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

/* Last 'ZB_NVRAM_PAGE_COUNT' indexes will store ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR of each page */
/* index is used for nvsId, datasetInfo is stored in MAX_DATASETS nvsId */
#define MAX_DATASETS              ZB_NVRAM_DATASET_NUMBER+ZB_NVRAM_PAGE_COUNT
#define NVSID_TO_DATASETTYPE(idx) (idx < ZB_NVRAM_DATASET_NUMBER)?(idx):(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR)
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
};

#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
/* Expose the mounted NVS instance so the ELE driver can persist the master-key
 * blob in the same partition. Valid only after zb_osif_nvram_open(). */
struct nvs_fs *zb_osif_nvram_get_fs(void)
{
  return &gc_flash_nvram.fs;
}
#endif

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


#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
static zb_uint8_t *localEncBuf = NULL;
/* Defined in zb_ele_s200_driver.c; header not delivered in this tree. */
extern zb_ret_t zb_ele_master_key_boot(void);
extern zb_ret_t zb_ele_master_key_erase(void);
extern zb_ret_t zb_ele_master_key_reset(void);
#endif

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
  /* The single NVS filesystem must span the whole zb_nvram partition, i.e. all
   * ZB_NVRAM_PAGE_COUNT logical pages (each ZB_NVRAM_PAGE_SIZE). Using only
   * ZB_NVRAM_PAGE_SIZE/info.size mounts just the first page, so the ZBOSS core
   * (told via zb_get_nvram_page_length()*count that it has the full size)
   * addresses the 2nd page outside the mount -> OUT_OF_RANGE at init. */
  flash->fs.sector_count = (ZB_NVRAM_PAGE_SIZE * ZB_NVRAM_PAGE_COUNT) / info.size;

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
  zb_uint8_t  curPage = 0;
  zb_uint32_t curPos = 0;
  zb_uint16_t nvsId   = 0;
  zb_nvram_dataset_hdr_t datasetHeader;

  ZVUNUSED(name);

  if (nvram_inited)
    return;

  ret = zb_osif_nvram_open(&gc_flash_nvram);
  if(ret != 0) {
    add_info.line_number = __LINE__;
    zb_error_raise(ZB_ERROR_SEVERITY_FATAL, RET_NO_RESOURCES, &add_info);
  }

  localNvsSize += LOCAL_BUF_ALLOC_SIZE;
  localNvsBuf = ZB_MALLOC(localNvsSize);
#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
  localEncBuf = ZB_MALLOC(localNvsSize + zb_nvram_crypto_overhead());
#endif

  /* Rebuild table mapping between position & nvs_id */
  for(nvsId = ZB_NVRAM_DATASET_NUMBER; nvsId < MAX_DATASETS; nvsId++) {
    ret = nvs_read(&gc_flash_nvram.fs, nvsId, &datasetHeader, sizeof(datasetHeader));
    if(ret > 0 && ret >= sizeof(datasetHeader)) {
      datasetInfo[nvsId].page = nvsId - ZB_NVRAM_DATASET_NUMBER;
      datasetInfo[nvsId].pos  = 0;
      datasetInfo[nvsId].len  = datasetHeader.data_len;
      datasetInfo[nvsId].ver  = datasetHeader.data_set_version;
#ifdef DEBUG_NVS
      WCS_TRACE_WARNING("%s() rebuild page %d, pos 0x%04x, len 0x%04x (%s), ver %d for %s",
        __FUNCTION__,
        datasetInfo[nvsId].page, datasetInfo[nvsId].pos,
        datasetHeader.data_len, (ret == datasetHeader.data_len)?("ok"):("BAD LEN"), datasetHeader.data_set_version,
        get_nvram_dataset_str(ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR));
#endif
    } else {
      nvs_delete(&gc_flash_nvram.fs, nvsId);
    }
  }

  curPage  = 0;
  curPos = datasetInfo[ZB_NVRAM_DATASET_NUMBER+curPage].pos + datasetInfo[ZB_NVRAM_DATASET_NUMBER+curPage].len;
  if(curPos != 0) {
    /* We found pages header, so there can be some other datasets */
    for(nvsId = 0; nvsId < ZB_NVRAM_DATASET_NUMBER; nvsId++) {
      ret = nvs_read(&gc_flash_nvram.fs, nvsId, &datasetHeader, sizeof(datasetHeader));
      if(ret > 0 && ret >= sizeof(datasetHeader)) {
        datasetInfo[nvsId].page = curPage;
        datasetInfo[nvsId].pos  = curPos;
        datasetInfo[nvsId].len  = datasetHeader.data_len;
        datasetInfo[nvsId].ver  = datasetHeader.data_set_version;
#ifdef DEBUG_NVS
        WCS_TRACE_WARNING("%s() rebuild page %d, pos 0x%04x, len 0x%04x (%s), ver %d for %s",
          __FUNCTION__,
          datasetInfo[nvsId].page, datasetInfo[nvsId].pos,
          datasetHeader.data_len, (ret == datasetHeader.data_len)?("ok"):("BAD LEN"), datasetHeader.data_set_version,
          get_nvram_dataset_str(nvsId));
#endif
        curPos += datasetHeader.data_len;
        /* Switch to next page if needed */
        if(curPos + sizeof(datasetHeader) > ZB_NVRAM_PAGE_SIZE) {
          curPage++;
          if(curPage >= ZB_NVRAM_PAGE_COUNT) {
            add_info.line_number = __LINE__;
            zb_error_raise(ZB_ERROR_SEVERITY_FATAL, RET_OUT_OF_RANGE, &add_info);
          }
          curPos = datasetInfo[ZB_NVRAM_DATASET_NUMBER+curPage].pos + datasetInfo[ZB_NVRAM_DATASET_NUMBER+curPage].len;
        }
      } else {
        nvs_delete(&gc_flash_nvram.fs, nvsId);
      }
    }
  }

#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
  /* NVS is mounted now: provision or restore the ELE master key before any
   * encrypted dataset is decrypted. Crypto HW was brought up by zb_ele_init(). */
  if (zb_ele_master_key_boot() != RET_OK) {
    add_info.line_number = __LINE__;
    zb_error_raise(ZB_ERROR_SEVERITY_FATAL, RET_ERROR, &add_info);
  }
#endif

#ifdef ZB_PRODUCTION_CONFIG
  zb_osif_prod_cfg_init();
#endif

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
      if(relocateDataset != (zb_uint16_t)-1)
        ZB_MEMCPY(&relocateOldInfo, &datasetInfo[idx], sizeof(relocateOldInfo));
      datasetInfo[idx].page = page;
      datasetInfo[idx].pos  = pos;
      datasetInfo[idx].ver  = dataset->data_set_version;
      datasetInfo[idx].len  = dataset->data_len;
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
    zb_uint8_t *writeBuf = localNvsBuf;
    zb_uint16_t writeLen = datasetInfo[localNvsId].len;

#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
    if(zb_nvram_dataset_is_encrypted(NVSID_TO_DATASETTYPE(localNvsId))) {
      writeBuf = localEncBuf;
      writeLen = zb_nvram_dataset_encrypt(localNvsBuf, datasetInfo[localNvsId].len,
                                          writeBuf, localNvsSize + zb_nvram_crypto_overhead(),
                                          sizeof(zb_nvram_dataset_hdr_t));
      if(writeLen != datasetInfo[localNvsId].len + zb_nvram_crypto_overhead()) {
        ZB_ASSERT(false);
      }
#ifdef DEBUG_NVS
      WCS_TRACE_DEBUG("dataset %s encrypt %d -> %d",
        get_nvram_dataset_str(NVSID_TO_DATASETTYPE(localNvsId)),
        datasetInfo[localNvsId].len, writeLen);
      DEBUG_PRINT_BUFFER(localNvsBuf, datasetInfo[localNvsId].len, "%s-encrypt()", __FUNCTION__, datasetInfo[localNvsId].len);
#endif
    }
#endif

    ret = nvs_write(&gc_flash_nvram.fs, localNvsId, writeBuf, writeLen);
    if(ret < writeLen && ret >= 0)
    {
#ifdef DEBUG_NVS
    WCS_TRACE_ERROR("dataset %s nvs_write(--, %d, --, %d) ret %d, should be %d, DELETE IT!",
      get_nvram_dataset_str(NVSID_TO_DATASETTYPE(localNvsId)),
      localNvsId, writeLen, ret, writeLen);
#endif
      nvs_delete(&gc_flash_nvram.fs, localNvsId);
      ret = nvs_write(&gc_flash_nvram.fs, localNvsId, writeBuf, writeLen);
    }
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("dataset %s nvs_write(--, %d, --, %d) ret %d",
      get_nvram_dataset_str(NVSID_TO_DATASETTYPE(localNvsId)),
      localNvsId, writeLen, ret);
    DEBUG_PRINT_BUFFER(writeBuf, writeLen, "%s-nvs_write(--, %d, --, %d)", __FUNCTION__, localNvsId, writeLen);
#endif

    localNvsFlush = ZB_FALSE;
  }

  return ret;
}

static int do_localNvs_read(zb_uint16_t nvsId, zb_uint16_t oldLen)
{
  int ret = 0;

  if((localNvsId != nvsId) || (localNvsSize < datasetInfo[nvsId].len)) {
    zb_uint8_t *readBuf = localNvsBuf;
    zb_uint16_t readLen = (oldLen)?(oldLen):(datasetInfo[nvsId].len);

    /* In case we change the dataset before doing the write tail (that trigs the write) */
    do_localNvs_write();

    localNvsId = nvsId;
    if(localNvsSize < datasetInfo[localNvsId].len) {
      WCS_TRACE_ERROR("Re-allocate localNvsBuf len %d -> %d, current dataset needs %d", localNvsSize, localNvsSize + LOCAL_BUF_ALLOC_SIZE, datasetInfo[localNvsId].len);
      /* When allocating at runtime (not at init), we faced issue "heap corruption" elsewhere...  */
      localNvsSize += LOCAL_BUF_ALLOC_SIZE;
      localNvsBuf = ZB_REALLOC(localNvsBuf, localNvsSize);
#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
      localEncBuf = ZB_REALLOC(localEncBuf, localNvsSize + zb_nvram_crypto_overhead());
#endif
    }

#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
    if(zb_nvram_dataset_is_encrypted(NVSID_TO_DATASETTYPE(localNvsId))) {
      readBuf = localEncBuf;
      readLen += zb_nvram_crypto_overhead();
    }
#endif

    ret = nvs_read(&gc_flash_nvram.fs, localNvsId, readBuf, readLen);
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("dataset %s nvs_read(--, %d, --, %d) ret %d",
      get_nvram_dataset_str(NVSID_TO_DATASETTYPE(localNvsId)),
      localNvsId, readLen, ret);
    DEBUG_PRINT_BUFFER(readBuf, readLen, "%s-nvs_read(--, %d, --, %d)", __FUNCTION__, localNvsId, readLen);
#endif
    if(ret < readLen) {
        if(ret < 0) ret = 0;
        /* Not found, fake an empty data */
        ZB_MEMSET(readBuf/*+ret*/, 0xFF, datasetInfo[localNvsId].len/* - ret*/);
    }
#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
    else if(zb_nvram_dataset_is_encrypted(NVSID_TO_DATASETTYPE(localNvsId))) {
      zb_uint16_t decLen = 0;

      decLen = zb_nvram_dataset_decrypt(readBuf, readLen, localNvsBuf, localNvsSize, sizeof(zb_nvram_dataset_hdr_t));
      if(decLen != readLen - zb_nvram_crypto_overhead()) {
        /* Decrypt failed, fake an empty data */
        ZB_MEMSET(localNvsBuf, 0xFF, datasetInfo[localNvsId].len);
        decLen = readLen - zb_nvram_crypto_overhead();
      }
#ifdef DEBUG_NVS
      WCS_TRACE_DEBUG("dataset %s decrypt %d -> %d",
        get_nvram_dataset_str(NVSID_TO_DATASETTYPE(localNvsId)),
        readLen, decLen);
      DEBUG_PRINT_BUFFER(localNvsBuf, decLen, "%s-decrypt()", __FUNCTION__);
#endif
      readLen = decLen;
    }
#endif
    if(readLen != datasetInfo[localNvsId].len) {
      /* Dataset len will change, delete it */
#ifdef DEBUG_NVS
    WCS_TRACE_DEBUG("dataset %s len changed, nvs_delete(--, %d) ret %d",
      get_nvram_dataset_str(NVSID_TO_DATASETTYPE(localNvsId)),
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

  data_set_type = NVSID_TO_DATASETTYPE(idx);
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

#ifdef ZB_CRYPTO_NXP_USE_EDGELOCK_SECURE_ENCLAVE
  /* Refresh the ELE master key on factory reset / erase-at-start. The blob
   * (NVS id 0x5A00) lives above the datasets, so the loop above skips it.
   * erase_async() runs once per page; refresh only on the last page, so all
   * pages are wiped and a valid new key is ready for the datasets re-written next. */
  if (page == (ZB_NVRAM_PAGE_COUNT - 1)) {
    (void)zb_ele_master_key_reset();
  }
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

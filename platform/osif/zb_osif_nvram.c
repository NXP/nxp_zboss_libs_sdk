/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2025-2026 NXP
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

#define ZB_TRACE_FILE_ID 34009
#include "zboss_api_core.h"
#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>

#undef DEBUG_NVRAM

/* Matches LFS_NAME_MAX */
#define MAX_PATH_LEN 255

#define PARTITION_NODE DT_NODELABEL(lfs1)

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
static struct fs_mount_t lfs_storage_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &storage,
	.storage_dev = (void *)FIXED_PARTITION_ID(storage_partition),
	.mnt_point = "/lfs",
};


#ifndef ZB_NVRAM_PAGE_SIZE
#define ZB_NVRAM_PAGE_SIZE   0x4000
#endif
#define ZB_NVRAM_PAGE_COUNT  2

/*! \addtogroup ZB_OSIF */
/*! @{ */

#if defined ZB_USE_NVRAM || defined DOXYGEN


struct fs_file_t nvram_file;
struct fs_file_t prod_file;


enum zb_osif_nvram_last_operation_e
{
  ZB_OSIF_NVRAM_LAST_OPERATION_UNDEFINED,
  ZB_OSIF_NVRAM_LAST_OPERATION_READ,
  ZB_OSIF_NVRAM_LAST_OPERATION_WRITE,
};

void zb_nvram_erase_finished(zb_uint8_t page);
static zb_uint8_t zb_osif_nvram_last_operation;


static zb_bool_t nvram_inited = ZB_FALSE;


static int zb_osif_nvram_flash_partition_init(void)
{
  const struct flash_area *pfa;
  unsigned int id = (uint8_t)lfs_storage_mnt.storage_dev;
  int ret;

  ret = flash_area_open(id, &pfa);
  if (ret < 0) {
    WCS_TRACE_ERROR("%s() ERROR flash_area_open(%u, --) failed: %d", __FUNCTION__, id, ret);
    return ret;
  }

  /* Optional wipe flash contents */
  if (IS_ENABLED(CONFIG_APP_WIPE_STORAGE)) {
    ret = flash_area_flatten(pfa, 0, pfa->fa_size);
  }

  flash_area_close(pfa);

  if (ret < 0) {
    WCS_TRACE_ERROR("%s() ERROR flash_area_flatten() failed: %d", __FUNCTION__, id, ret);
    return ret;
  }

  ret = fs_mount(&lfs_storage_mnt);
  if (ret < 0) {
    WCS_TRACE_ERROR("%s() ERROR fs_mount(%s) failed: %d", __FUNCTION__, lfs_storage_mnt.mnt_point, ret);
    return ret;
  }

  return 0;
}

void zb_osif_nvram_init(const zb_char_t *name)
{
  int ret;
  off_t size;
  zb_uint8_t i;
  char fname[MAX_PATH_LEN];

  ZVUNUSED(name);

  if (nvram_inited)
    return;

  ret = zb_osif_nvram_flash_partition_init();
  if(ret < 0) {
    WCS_TRACE_ERROR("%s() mount failed %d", __FUNCTION__, ret);
    return;
  }

  snprintf(fname, sizeof(fname), "%s/zboss.nvram", lfs_storage_mnt.mnt_point);

  fs_file_t_init(&nvram_file);
  ret = fs_open(&nvram_file, fname, FS_O_CREATE | FS_O_RDWR);
  if(ret < 0) {
    WCS_TRACE_ERROR("Failed to open %s: %d: %s", fname, ret, strerror(-ret));
    return;
  }

  zb_osif_nvram_last_operation = ZB_OSIF_NVRAM_LAST_OPERATION_UNDEFINED;

#ifdef ZB_PRODUCTION_CONFIG
  fs_file_t_init(&prod_file);

  snprintf(fname, sizeof(fname), "%s/zboss.prod", lfs_storage_mnt.mnt_point);
  fs_open(&nvram_file, fname, FS_O_CREATE | FS_O_RDWR);
#endif

  fs_seek(&nvram_file, 0, FS_SEEK_END);
  size = fs_tell(&nvram_file);
  if(size < ZB_NVRAM_PAGE_SIZE*ZB_NVRAM_PAGE_COUNT) {
    WCS_TRACE_INFO("nvram size %d", size);
    for(i=0;i<ZB_NVRAM_PAGE_COUNT;i++)
    {
      zb_osif_nvram_erase_async(i);
    }
  }

    nvram_inited = ZB_TRUE;
}

void zb_osif_nvram_deinit(void)
{
  if(nvram_inited) {
    zb_osif_nvram_flush();
    fs_close(&nvram_file);
#ifdef ZB_PRODUCTION_CONFIG
    fs_close(&prod_file);
#endif
    fs_unmount(&lfs_storage_mnt);
    zb_osif_nvram_last_operation = ZB_OSIF_NVRAM_LAST_OPERATION_UNDEFINED;
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

zb_ret_t zb_osif_nvram_read(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len )
{
  int iRet;
  zb_ret_t zRet = RET_OK;
  zb_int32_t cur_pos;

  if(page>=ZB_NVRAM_PAGE_COUNT)
  {
    return RET_PAGE_NOT_FOUND;
  }
  if(pos+len>ZB_NVRAM_PAGE_SIZE || len == 0)
  {
    return RET_INVALID_PARAMETER;
  }

  if (zb_osif_nvram_last_operation != ZB_OSIF_NVRAM_LAST_OPERATION_READ)
  {
    zb_osif_nvram_last_operation = ZB_OSIF_NVRAM_LAST_OPERATION_READ;
    zb_osif_nvram_flush();
  }

  cur_pos = (zb_int32_t)fs_tell(&nvram_file);

  if ((zb_uint32_t)cur_pos != page*ZB_NVRAM_PAGE_SIZE+pos)
  {
    fs_seek(&nvram_file, page*ZB_NVRAM_PAGE_SIZE+pos, SEEK_SET);
    cur_pos = page*ZB_NVRAM_PAGE_SIZE+pos;
  }

  iRet = fs_read(&nvram_file, buf, len);
  if(iRet < len) {
    WCS_TRACE_ERROR("%s(%d, %d, --, %d) Error %d", __FUNCTION__, page, pos, len, iRet);
    zRet = RET_OPERATION_FAILED;
  }

#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s(%d, %d, --, %d) %s %s", __FUNCTION__, page, pos, len, wcs_get_error_str(zRet), (iRet<0)?(strerror(iRet)):(""));
//  wcs_print_buf(buf, len, "%s(%d, %d, --, %d) %s", __FUNCTION__, page, pos, len, wcs_get_error_str(zRet));
#endif
  return zRet;
}

zb_ret_t zb_osif_nvram_write(zb_uint8_t page, zb_uint32_t pos, void *buf, zb_uint16_t len )
{
  int iRet;
  zb_ret_t zRet = RET_OK;
  zb_int32_t cur_pos;

  if(page>=ZB_NVRAM_PAGE_COUNT)
  {
    return RET_PAGE_NOT_FOUND;
  }
  if(pos+len>ZB_NVRAM_PAGE_SIZE || len == 0)
  {
    return RET_INVALID_PARAMETER;
  }

  if (zb_osif_nvram_last_operation != ZB_OSIF_NVRAM_LAST_OPERATION_WRITE)
  {
    zb_osif_nvram_last_operation = ZB_OSIF_NVRAM_LAST_OPERATION_WRITE;
    zb_osif_nvram_flush();
  }

  cur_pos = (zb_int32_t)fs_tell(&nvram_file);

  if ((zb_uint32_t)cur_pos != page*ZB_NVRAM_PAGE_SIZE+pos)
  {
    fs_seek(&nvram_file, page*ZB_NVRAM_PAGE_SIZE+pos, SEEK_SET);
    cur_pos = page*ZB_NVRAM_PAGE_SIZE+pos;
  }

  iRet = fs_write(&nvram_file, buf, len);
  if(iRet < len) {
    WCS_TRACE_ERROR("%s(%d, %d, --, %d) Error %d", __FUNCTION__, page, pos, len, iRet);
    zRet = RET_OPERATION_FAILED;
  }

#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s(%d, %d, --, %d) %s %s", __FUNCTION__, page, pos, len, wcs_get_error_str(zRet), (iRet<0)?(strerror(iRet)):(""));
//  wcs_print_buf(buf, len, "%s(%d, %d, --, %d) %s", __FUNCTION__, page, pos, len, wcs_get_error_str(zRet));
#endif
  return zRet;
}

zb_ret_t zb_osif_nvram_erase_async(zb_uint8_t page)
{
  zb_uint8_t *buf;
  zb_uint32_t pos;

  if(page>=ZB_NVRAM_PAGE_COUNT)
  {
    return RET_PAGE_NOT_FOUND;
  }

  buf = ZB_MALLOC(256);
  if(!buf)
    return RET_NO_MEMORY;

  ZB_MEMSET(buf, 0xFF, 256);

  fs_seek(&nvram_file, page*ZB_NVRAM_PAGE_SIZE, SEEK_SET);
  for(pos = 0; pos < ZB_NVRAM_PAGE_SIZE; pos += 256) {
    ssize_t ret = fs_write(&nvram_file, buf, 256);
    if(ret < 256) {
      WCS_TRACE_WARNING("%s(%d) failed to write at %d offset %d: %d: %s", __FUNCTION__, page, pos, page*ZB_NVRAM_PAGE_SIZE, ret, (ret < 0)?(strerror(-ret)):(""));
      break;
    }
  }
  zb_osif_nvram_flush();
  zb_osif_nvram_last_operation = ZB_OSIF_NVRAM_LAST_OPERATION_WRITE;

  /* Really, erase is synchronous for file based nvram */
  zb_nvram_erase_finished(page);

  ZB_FREE(buf);

#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s(%d) %s (from %d to %d)", __FUNCTION__, page, wcs_get_error_str(RET_OK), page*ZB_NVRAM_PAGE_SIZE, (page+1)*ZB_NVRAM_PAGE_SIZE - 1);
#endif
  return RET_OK;
}

void zb_osif_nvram_flush()
{
  int ret = fs_sync(&nvram_file);
#ifdef DEBUG_NVRAM
  WCS_TRACE_WARNING("%s() %s", __FUNCTION__, strerror(ret));
#else
  ZVUNUSED(ret);
#endif
}
#endif


#ifdef ZB_PRODUCTION_CONFIG
zb_bool_t zb_osif_prod_cfg_check_presence()
{
  WCS_TRACE_ERROR("%s() STUBBED", __FUNCTION__);
  return ZB_FALSE;
}

zb_ret_t zb_osif_prod_cfg_read_header(zb_uint8_t *prod_cfg_hdr, zb_uint16_t hdr_len)
{
  WCS_TRACE_ERROR("%s() STUBBED", __FUNCTION__);
  ZVUNUSED(prod_cfg_hdr);
  ZVUNUSED(hdr_len);
  return RET_NOT_IMPLEMENTED;
}

zb_ret_t zb_osif_prod_cfg_read(zb_uint8_t *buffer, zb_uint16_t len, zb_uint16_t offset)
{
  WCS_TRACE_ERROR("%s() STUBBED", __FUNCTION__);
  ZVUNUSED(buffer);
  ZVUNUSED(len);
  ZVUNUSED(offset);
  return RET_NOT_IMPLEMENTED;
}
#endif
/*! @} */

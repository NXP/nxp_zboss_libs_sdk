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

#define ZB_TRACE_FILE_ID 34039
#include "zboss_api_core.h"
#include "zboss_api_error.h"
#include <errno.h>

#include "zboss_api.h"

#ifndef CONFIG_BOARD_FRDM_RW612

#include <zephyr/device.h>
#include <zephyr/nvmem.h>


/* Zboss production config Mapping:
 * total 349 bytes including:
 * - header:          8 bytes
 * - channels masks: 40 bytes (10 elemnts of 4 bytes)
 * - extended addr:   8 bytes                             => shared with ieee addr
 * - tx_pwr:        270 bytes (10 elemnts of 27 bytes)
 * - options:         1 byte
 * - install_code:   18 bytes
 * - passcode:        4 bytes
 */



/*! \addtogroup ZB_OSIF */
/*! @{ */

#ifdef ZB_PRODUCTION_CONFIG

/* Get NVMEM cells from device tree */
static const struct nvmem_cell eui64_cell       = NVMEM_CELL_GET_BY_NAME(DT_NODELABEL(hw_params), ieee802154_eui64); /* Shared with OpenThread who can generate it */
static const struct nvmem_cell zb_settings_cell = NVMEM_CELL_GET_BY_NAME(DT_NODELABEL(hw_params), zb_settings);
static const struct nvmem_cell zb_secured_cell  = NVMEM_CELL_GET_BY_NAME(DT_NODELABEL(hw_params), zb_secured);

/* Hardware parameters partition and offsets */
#define MAC_ADDRESS_LEN        8                    // => 8

#define ZBOSS_HEADER_LEN       8                    // => 8
#define ZBOSS_CHMASKS_LEN      40                   // => 48
#define ZBOSS_TXPOWER_LEN      270                  // => 318
#define ZBOSS_OPTIONS_LEN      1                    // => 319
// padding 1:     size becomes 320
// extra padding for futur consideration:
// expadding 176: size becomes 496 (0x1F0)
#define ZB_SETTINGS_PADDING 177

#define ZBOSS_INSTALLCODE_UNS_LEN  18                   // => 18
#define ZBOSS_PASSCODE_UNS_LEN     4                    // => 22
#define ZBOSS_INSTALLCODE_LEN      36                   // => 36
#define ZBOSS_PASSCODE_LEN         8                    // => 44
// padding 4: size becomes 48
// extra padding for futur consideration:
// expadding 80: size becomes 128 (0x80)
#define ZB_SECURED_PADDING 84


typedef ZB_PACKED_PRE struct hw_params_s
{
  /* Created for IEEE802154 address, to be stored in IFR0 */
  struct {
    uint8_t mac_addr[MAC_ADDRESS_LEN];
  } ieee802154_eui64;

  /* Padding 8 bytes */

  /* Created for Zboss, to be stored in IFR0 */
  struct {
    uint8_t header[ZBOSS_HEADER_LEN];
    uint8_t ch_masks[ZBOSS_CHMASKS_LEN];
    uint8_t tx_power[ZBOSS_TXPOWER_LEN];
    uint8_t options[ZBOSS_OPTIONS_LEN];
    uint8_t padding[ZB_SETTINGS_PADDING];
  } zb_settings;

  /* Created for Zboss, to be stored in blobs */
  struct {
    uint8_t installcode[ZBOSS_INSTALLCODE_LEN];
    uint8_t passcode[ZBOSS_PASSCODE_LEN];
    uint8_t padding[ZB_SECURED_PADDING];
  } zb_secured;
} ZB_PACKED_STRUCT hw_params_t;

static zb_bool_t gc_prod_cfg_presence = ZB_FALSE;
static zb_production_config_ver_4_t gc_prod_cfg = {0};

void zb_osif_prod_cfg_init(void)
{
}

void zb_osif_prod_cfg_deinit(void)
{
}


zb_bool_t zb_osif_prod_cfg_check_presence()
{
  if(!gc_prod_cfg_presence) {
    int ret;
    hw_params_t *hw_params =ZB_MALLOC(sizeof(hw_params_t));

    if(!hw_params)
      return ZB_FALSE;

    ret = nvmem_cell_read(&eui64_cell, &hw_params->ieee802154_eui64, 0, sizeof(hw_params->ieee802154_eui64));
    if(ret != 0) {
        WCS_TRACE_WARNING("read cell hw_params->ieee802154_eui64 error: %d", ret);
        ZB_FREE(hw_params);
        return ZB_FALSE;
    }

    ret = nvmem_cell_read(&zb_settings_cell, &hw_params->zb_settings, 0, sizeof(hw_params->zb_settings));
    if(ret != 0) {
        WCS_TRACE_WARNING("read cell hw_params->zb_settings error: %d", ret);
        ZB_FREE(hw_params);
        return ZB_FALSE;
    }

    ret = nvmem_cell_read(&zb_secured_cell, &hw_params->zb_secured, 0, sizeof(hw_params->zb_secured));
    if(ret != 0) {
        WCS_TRACE_WARNING("read cell hw_params->zb_secured error: %d", ret);
        ZB_FREE(hw_params);
        return ZB_FALSE;
    }

    /* Rebuild zboss struct
     * since flash mapping and zboss production_config struct does not have the same layout
     */
    ZB_MEMCPY(&gc_prod_cfg.hdr,                  hw_params->zb_settings.header,     ZBOSS_HEADER_LEN);
    ZB_MEMCPY(gc_prod_cfg.aps_channel_mask_list, hw_params->zb_settings.ch_masks,   ZBOSS_CHMASKS_LEN);
    /* mac_addr is in first place on flash and is shared with ieee802154 driver */
    ZB_MEMCPY(&gc_prod_cfg.extended_address,     hw_params->ieee802154_eui64.mac_addr,    MAC_ADDRESS_LEN);
    ZB_MEMCPY(gc_prod_cfg.mac_tx_power,          hw_params->zb_settings.tx_power,   ZBOSS_TXPOWER_LEN);
    ZB_MEMCPY(&gc_prod_cfg.options,              hw_params->zb_settings.options,    ZBOSS_OPTIONS_LEN);
    // TODO: uncrypt installcode
    ZB_MEMCPY(gc_prod_cfg.install_code,          hw_params->zb_secured.installcode, ZBOSS_INSTALLCODE_UNS_LEN);
    // TODO: uncrypt passcode
    ZB_MEMCPY(gc_prod_cfg.passcode,              hw_params->zb_secured.passcode,    ZBOSS_PASSCODE_UNS_LEN);

    WCS_TRACE_INFO("read hw_params: zboss ver 0x%04x (len 0x%04x), ieee_addr "WCS_TRACE_FORMAT_64,
      gc_prod_cfg.hdr.version, gc_prod_cfg.hdr.len,
      WCS_TRACE_ARG_64(gc_prod_cfg.extended_address));

    gc_prod_cfg_presence = ZB_TRUE;

    ZB_FREE(hw_params);
  }

  return gc_prod_cfg_presence;
}

zb_ret_t zb_osif_prod_cfg_read_header(zb_uint8_t *prod_cfg_hdr, zb_uint16_t hdr_len)
{
  return zb_osif_prod_cfg_read(prod_cfg_hdr, hdr_len, 0);
}

zb_ret_t zb_osif_prod_cfg_read(zb_uint8_t *buffer, zb_uint16_t len, zb_uint16_t offset)
{
  zb_uint8_t *data = (zb_uint8_t *)&gc_prod_cfg;

  if(!gc_prod_cfg_presence)
    return RET_NO_RESOURCES;

  if(!buffer)
    return RET_INVALID_PARAMETER_1;

  if(offset + len > sizeof(gc_prod_cfg))
    return RET_INVALID_PARAMETER_2;

  ZB_MEMCPY(buffer, data + offset, len);

  return RET_OK;
}
#endif

#else /* CONFIG_BOARD_FRDM_RW612 */

#include <zephyr/storage/flash_map.h>


/* Zboss production config Mapping:
 * total 349 bytes including:
 * - header:          8 bytes
 * - channels masks: 40 bytes (10 elemnts of 4 bytes)
 * - extended addr:   8 bytes                             => shared with ieee addr ?
 * - tx_pwr:        270 bytes (10 elemnts of 27 bytes)
 * - options:         1 byte
 * - install_code:   18 bytes                             => secure storage ? / Blobs ? => request 5200 (Philippe Guasch)
 * - passcode:        4 bytes                             => secure storage ? / Blobs ? => request 5200 (Philippe Guasch)
 *
 * padding: 13 => to have size 362 (multiple of 16).
 */



/*! \addtogroup ZB_OSIF */
/*! @{ */

#ifdef ZB_PRODUCTION_CONFIG

/* Hardware parameters partition and offsets */
#define HW_PARAMS_PARTITION_ID PARTITION_ID(hw_params_partition)

#define MAC_ADDRESS_OFFSET     0x00
#define MAC_ADDRESS_LEN        8

#define ZBOSS_HEADER_LEN       8
#define ZBOSS_CHMASKS_LEN      40
#define ZBOSS_TXPOWER_LEN      270
#define ZBOSS_OPTIONS_LEN      1
#define ZBOSS_INSTALLCODE_LEN  18
#define ZBOSS_PASSCODE_LEN     4


// TODO: removes ZB_PACKED_PRE & ZB_PACKED_STRUCT to be exported into zephyr hal_nxp
typedef ZB_PACKED_PRE struct hw_params_s
{
  /* Created for IEEE802154 address, to be stored in EFR0 */
  uint8_t mac_addr[MAC_ADDRESS_LEN];

  /* Created for Zboss, to be stored in EFR0 */
  uint8_t zboss_header[ZBOSS_HEADER_LEN];
  uint8_t zboss_ch_masks[ZBOSS_CHMASKS_LEN];
  uint8_t zboss_tw_power[ZBOSS_TXPOWER_LEN];
  uint8_t zboss_options[ZBOSS_OPTIONS_LEN];

  /* Created for Zboss, to be stored in blobs */
  uint8_t zboss_installcode[ZBOSS_INSTALLCODE_LEN];
  uint8_t zboss_passcode[ZBOSS_PASSCODE_LEN];
} ZB_PACKED_STRUCT hw_params_t;

typedef struct {
  const struct flash_area     *fa;
  zb_uint32_t                  align;
  zb_uint8_t                   id;
  zb_bool_t                    presence;
} zephyr_flash_t;

static zephyr_flash_t gc_hw_params = {
  .id       = HW_PARAMS_PARTITION_ID,
  .fa       = NULL,
  .presence = ZB_FALSE,
};
static zb_production_config_ver_4_t gc_prod_cfg = {0};

void zb_osif_prod_cfg_init(void)
{
  int ret = 0;

  if(sizeof(hw_params_t) != sizeof(zb_production_config_ver_4_t)) {
    WCS_TRACE_ERROR("%s() FATAL: size of data doesn't match", __FUNCTION__);
    while(1);
  }

  ret = flash_area_open(gc_hw_params.id, &gc_hw_params.fa);
  if(ret != 0) {
    WCS_TRACE_ERROR("%s() ERROR: cannot open flash area hw_params: %s", __FUNCTION__, strerror(-ret));
    return;
  }
}

void zb_osif_prod_cfg_deinit(void)
{
    flash_area_close(gc_hw_params.fa);
    gc_hw_params.fa = NULL;
}


zb_bool_t zb_osif_prod_cfg_check_presence()
{
  if(!gc_hw_params.presence) {
    int ret;
    hw_params_t *hw_params =ZB_MALLOC(sizeof(hw_params_t));

    if(!hw_params)
      return ZB_FALSE;

    ret = flash_area_read(gc_hw_params.fa, 0, hw_params, sizeof(hw_params_t));
    if(ret != 0) {
        WCS_TRACE_WARNING("read hw_params error: %d", ret);
        ZB_FREE(hw_params);
        return ZB_FALSE;
    }

    /* Rebuild zboss struct
     * since flash mapping and zboss production_config struct does not have the same layout
     */
    ZB_MEMCPY(&gc_prod_cfg.hdr,                  hw_params->zboss_header,      ZBOSS_HEADER_LEN);
    ZB_MEMCPY(gc_prod_cfg.aps_channel_mask_list, hw_params->zboss_ch_masks,    ZBOSS_CHMASKS_LEN);
    /* mac_addr is in first place on flash and is shared with ieee802154 driver */
    ZB_MEMCPY(&gc_prod_cfg.extended_address,     hw_params->mac_addr,          MAC_ADDRESS_LEN);
    ZB_MEMCPY(gc_prod_cfg.mac_tx_power,          hw_params->zboss_tw_power,    ZBOSS_TXPOWER_LEN);
    ZB_MEMCPY(&gc_prod_cfg.options,              hw_params->zboss_options,     ZBOSS_OPTIONS_LEN);
    ZB_MEMCPY(gc_prod_cfg.install_code,          hw_params->zboss_installcode, ZBOSS_INSTALLCODE_LEN);
    ZB_MEMCPY(gc_prod_cfg.passcode,              hw_params->zboss_passcode,    ZBOSS_PASSCODE_LEN);

    WCS_TRACE_INFO("read hw_params: zboss ver %d (len %d), ieee_addr "WCS_TRACE_FORMAT_64,
      gc_prod_cfg.hdr.version, gc_prod_cfg.hdr.len,
      WCS_TRACE_ARG_64(gc_prod_cfg.extended_address));

    gc_hw_params.presence = ZB_TRUE;

    ZB_FREE(hw_params);
  }

  return gc_hw_params.presence;
}

zb_ret_t zb_osif_prod_cfg_read_header(zb_uint8_t *prod_cfg_hdr, zb_uint16_t hdr_len)
{
  return zb_osif_prod_cfg_read(prod_cfg_hdr, hdr_len, 0);
}

zb_ret_t zb_osif_prod_cfg_read(zb_uint8_t *buffer, zb_uint16_t len, zb_uint16_t offset)
{
  zb_uint8_t *data = (zb_uint8_t *)&gc_prod_cfg;

  if(!gc_hw_params.presence)
    return RET_NO_RESOURCES;

  if(!buffer)
    return RET_INVALID_PARAMETER_1;

  if(offset + len > sizeof(gc_prod_cfg))
    return RET_INVALID_PARAMETER_2;

  ZB_MEMCPY(buffer, data + offset, len);

  return RET_OK;
}
#endif

#endif /* CONFIG_BOARD_FRDM_RW612 */
/*! @} */

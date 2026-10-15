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
/* PURPOSE: ZBOSS production config backend reading from NVMEM cells declared
 * in the devicetree hw_params node. Used on MCXW71 / MCXW72.
 */

#define ZB_TRACE_FILE_ID 34039
#include "zboss_api_core.h"
#include "zboss_api_error.h"
#include <errno.h>

#include "zboss_api.h"

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
// crypto_overhead: 0x10: size becomes 64
// extra padding for futur consideration:
// expadding 64: size becomes 128 (0x80)
#define ZB_SECURED_PADDING 68


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

  /* Created for Zboss, stored in plaintext in IFR0 (no encryption).
   * Layout matches the flashed zb_secured cell: installcode(18)+passcode(4). */
  struct {
    uint8_t installcode[ZBOSS_INSTALLCODE_UNS_LEN];
    uint8_t passcode[ZBOSS_PASSCODE_UNS_LEN];
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
    hw_params_t *hw_params = ZB_MALLOC(sizeof(hw_params_t));

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

    /* zb_secured (install code + passcode) is stored in plaintext in IFR0. */
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
    ZB_MEMCPY(gc_prod_cfg.install_code,          hw_params->zb_secured.installcode, ZBOSS_INSTALLCODE_UNS_LEN);
    ZB_MEMCPY(gc_prod_cfg.passcode,              hw_params->zb_secured.passcode,    ZBOSS_PASSCODE_UNS_LEN);

    if (gc_prod_cfg.hdr.version != 0xFFFFU) {
      WCS_TRACE_INFO("hw_params: zboss prod ver 0x%04x (len 0x%04x), ieee_addr "WCS_TRACE_FORMAT_64
                     ", channel_mask: %08x",
        gc_prod_cfg.hdr.version, gc_prod_cfg.hdr.len,
        WCS_TRACE_ARG_64(gc_prod_cfg.extended_address),
        gc_prod_cfg.aps_channel_mask_list[0]);
    }

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
/*! @} */

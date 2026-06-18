/*
 * Copyright 2024-2026 NXP
 *
 * NXP Proprietary.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 *
 */

#ifndef CLI_NXP_CLUSTER_H
#define CLI_NXP_CLUSTER_H 1

#include "zboss_api.h"


#define CLI_HAS_CLUSTER_BASIC         1
#define CLI_HAS_CLUSTER_IDENTITY      1 /* has commands */
#define CLI_HAS_CLUSTER_GROUPS        1 /* has commands */
#define CLI_HAS_CLUSTER_SCENES        1 /* has commands */
#define CLI_HAS_CLUSTER_ONOFF         1 /* has commands */
#define CLI_HAS_CLUSTER_ONOFF_SWITCH  1
#define CLI_HAS_CLUSTER_TEMPERATURE   1
#define CLI_HAS_CLUSTER_OTA_UPGRADE_SRV 1 /* has commands, uses filesystem */
#define CLI_HAS_CLUSTER_OTA_UPGRADE_CLT 1 /* has commands, uses filesystem */
#define CLI_HAS_CLUSTER_THERMOSTAT    1 /* has commands */
#define CLI_HAS_CLUSTER_MANUF_SPE     1 /* has commands */

#ifdef ZB_PLATFORM_LINUX
//#undef CLI_HAS_CLUSTER_BASIC
//#undef CLI_HAS_CLUSTER_IDENTITY
//#undef CLI_HAS_CLUSTER_GROUPS
//#undef CLI_HAS_CLUSTER_SCENES
//#undef CLI_HAS_CLUSTER_ONOFF
//#undef CLI_HAS_CLUSTER_ONOFF_SWITCH
//#undef CLI_HAS_CLUSTER_TEMPERATURE
//#undef CLI_HAS_CLUSTER_OTA_UPGRADE_SRV
//#undef CLI_HAS_CLUSTER_OTA_UPGRADE_CLT
//#undef CLI_HAS_CLUSTER_THERMOSTAT
//#undef CLI_HAS_CLUSTER_MANUF_SPE

#define MAX_CLUSTERS 256
#endif

#ifdef ZB_PLATFORM_ZEPHYR
//#undef CLI_HAS_CLUSTER_BASIC
//#undef CLI_HAS_CLUSTER_IDENTITY
#undef CLI_HAS_CLUSTER_GROUPS
#undef CLI_HAS_CLUSTER_SCENES
//#undef CLI_HAS_CLUSTER_ONOFF
//#undef CLI_HAS_CLUSTER_ONOFF_SWITCH
//#undef CLI_HAS_CLUSTER_TEMPERATURE
#undef CLI_HAS_CLUSTER_OTA_UPGRADE_SRV
#undef CLI_HAS_CLUSTER_OTA_UPGRADE_CLT
//#undef CLI_HAS_CLUSTER_THERMOSTAT
//#undef CLI_HAS_CLUSTER_MANUF_SPE

#define MAX_CLUSTERS 8
#endif


/* if it returns false, the command will be manage by the stack
 * if it returns true, the command will be ignore by the stack */
typedef zb_uint8_t (*commands_handler_t)(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);


typedef struct {
  zb_zcl_attr_t         *attr_desc_list;
  zb_uint16_t            attr_count;
  zb_zcl_cluster_init_t  init_server;
  zb_zcl_cluster_init_t  init_client;
  commands_handler_t     commands_handler;
} zb_cluster_def;

typedef struct {
  zb_uint16_t     id;
  char           *domain;
  char           *name;
  char           *initials;
  char           *align;
  zb_cluster_def *definition;
} zb_cluster_entry;

typedef struct {
  zb_uint16_t dest_addr;
  zb_uint8_t dest_ep;
  zb_uint8_t src_ep;
  zb_uint16_t profile_id;
  zb_uint16_t cluster_id;
  zb_uint16_t manuf_code;
  zb_uint8_t max_count;
  zb_uint16_t total_attrs;
  zb_uint16_t next_attr_id;
  zb_uint16_t start_attr_id;
  zb_bool_t use_extra_options;
  zb_uint8_t aps_secured;
  zb_uint8_t disable_aps_ack;
  zb_uint16_t delay;
} disc_attr_ctx_t;

extern disc_attr_ctx_t g_disc_attr_ctx;

extern zb_cluster_entry table_clusters[];

extern cli_menu_cmd menu_cluster[];

void help_clusters(void);

void cluster_init(uint8_t ep_id);

void cluster_attributes_cb(zb_uint8_t param);

zb_uint8_t cluster_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

int wcs_snprintf_attr_val(char *str, size_t size, zb_uint8_t type, zb_uint8_t *value);
int wcs_snprintf_attr_raw(char *str, size_t size, zb_uint8_t type, zb_uint8_t *value);

void continue_disc_attr_wrapper(zb_uint8_t param);

#endif /* CLI_NXP_CLUSTER_H */


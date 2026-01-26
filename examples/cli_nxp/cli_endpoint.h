/*
 * Copyright 2024 NXP
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

#ifndef CLI_NXP_ENDPOINT_H
#define CLI_NXP_ENDPOINT_H 1

#include "zboss_api.h"
#include "cli_menu.h"

#ifdef ZB_PLATFORM_LINUX
#define MAX_ENDPOINTS 256
#endif
#ifdef ZB_PLATFORM_ZEPHYR
#define MAX_ENDPOINTS 8
#endif

typedef struct {
  zb_uint16_t         id;
  char               *name;
  char               *initials;
  char               *align;
} zb_endpoint_profile;

typedef struct {
  zb_uint16_t  id;
  char        *group;
  char        *name;
  char        *initials;
  char        *align;
} zb_endpoint_device;

extern zb_endpoint_profile table_profiles[];
extern zb_endpoint_device endpoint_devices[];

void help_profiles(void);


extern cli_menu_cmd menu_endpoint[];

/* Context containing endpoints table, each endpoint containing clusters table */
extern zb_af_device_ctx_t cli_ctx;

zb_af_endpoint_desc_t *endpoint_get_by_id(zb_uint8_t ep_id);


#endif /* CLI_NXP_ENDPOINT_H */


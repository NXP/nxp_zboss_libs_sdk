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

#ifndef CLI_NXP_CONFIG_H
#define CLI_NXP_CONFIG_H 1

#include "zboss_api.h"
#include "cli_menu.h"

typedef enum {
  STATE_INIT = 0, /* Doing ZB_INIT() */
  STATE_INITTING, /* Waiting for config to be done and start */
  STATE_RUN,      /* Doing zboss_start_no_autostart() */
  STATE_RUNNING,  /* Doing zboss_main_loop_iteration(), interrupted by config stop */
#ifdef ZB_ZBOSS_DEINIT
  STATE_STOP,     /* Doing zboss_start_shut() */
  STATE_STOPPING  /* Doing zboss_main_loop_iteration(), waiting for signal ZB_SIGNAL_READY_TO_SHUT */
#endif
} zb_cfg_state_e;



extern cli_menu_cmd menu_config[];

void config_init_default(void);
zb_cfg_state_e config_get_state(void);
#define config_is_started() (config_get_state() == STATE_RUNNING)
void config_set_state(zb_cfg_state_e state);
zb_bool_t config_is_steering_received(void);
zb_uint8_t config_get_permit_join_duration(void);
void config_got_signal(zb_zdo_app_signal_type_t sig, zb_zdo_app_signal_hdr_t *signal_hdr);
#if defined(ZB_COORDINATOR_ROLE)
zb_bool_t config_is_coordinator(void);
#endif
#if defined(ZB_COORDINATOR_ROLE) || defined(ZB_ROUTER_ROLE)
zb_bool_t config_is_concentrator(void);
#endif
#if defined(ZB_ROUTER_ROLE)
zb_bool_t config_is_router(void);
#endif
#if defined(ZB_ED_ROLE)
zb_bool_t config_is_end_device(void);
#endif
zb_ieee_addr_t *config_get_ieee_addr(void);
#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
zb_bool_t config_update(zb_uint8_t channel, zb_uint16_t panid, zb_ext_pan_id_t ext_panid);
#endif

#endif /* CLI_NXP_CONFIG_H */


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
/* PURPOSE: ZB Simple switch device
*/

#define ZB_TRACE_FILE_ID 40146
#include "zboss_api.h"
#include "zb_led_button.h"

#ifdef ZB_CONFIGURABLE_MEM
#include "zb_mem_config_min.h"
#endif


#if ! defined ZB_ED_FUNC
#error define ZB_ED_ROLE to compile ze tests
#endif

/**
 * Global variables definitions
 */


zb_ieee_addr_t g_ed_addr = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}; /* IEEE address of the
                                                                              * device */
/* Used endpoint */
#define MY_ENDPOINT_NUMBER          10

#if 0
/* Handler for specific ZCL commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);
#endif

/** [COMMON_DECLARATION] */
/**
 * Declaring attributes for each cluster
 */

/* Switch config cluster attributes */
zb_uint8_t attr_switch_type    = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_TOGGLE;
zb_uint8_t attr_switch_actions = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_DEFAULT_VALUE;
ZB_ZCL_DECLARE_ON_OFF_SWITCH_CONFIGURATION_ATTRIB_LIST(switch_cfg_attr_list,
                                                       &attr_switch_type,
                                                       &attr_switch_actions);

/* Basic cluster attributes */
zb_uint8_t attr_zcl_version  = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
zb_uint8_t attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &attr_zcl_version, &attr_power_source);

/* Identify cluster attributes */
zb_uint16_t attr_identify_time = 0;
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &attr_identify_time);


/* Declare cluster list for the device */
ZB_HA_DECLARE_ON_OFF_SWITCH_CLUSTER_LIST(my_endpoint_layout,
                                         switch_cfg_attr_list,
                                         basic_attr_list,
                                         identify_attr_list);
/* Declare endpoint */
ZB_HA_DECLARE_ON_OFF_SWITCH_EP(my_endpoint_definition, MY_ENDPOINT_NUMBER, my_endpoint_layout);

/* Declare application's device context for single-endpoint device */
ZBOSS_DECLARE_DEVICE_CTX_1_EP(my_application_ctx, my_endpoint_definition);
/** [COMMON_DECLARATION] */


MAIN()
{
  ARGV_UNUSED;

  /* Trace enable */
  ZB_SET_TRACE_ON();
  /* Traffic dump enable */
  ZB_SET_TRAF_DUMP_ON();

  /* Global ZBOSS initialization */
  ZB_INIT("minimal_zed");

  /* Set up defaults for the commissioning */
  zb_set_long_address(g_ed_addr);
  zb_set_network_ed_role(1l<<21);

  zb_set_nvram_erase_at_start(ZB_FALSE);

#if 0
  /* Set end-device configuration parameters */
  zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
  zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(30000));
  zb_set_rx_on_when_idle(ZB_FALSE);
#endif

#ifdef ZB_PLATFORM_ZEPHYR
  /* Zigbee 3.0 compliant device */
  zboss_use_r22_behavior();
#endif

/** [REGISTER] */
  /* Register device ZCL context */
  ZB_AF_REGISTER_DEVICE_CTX(&my_application_ctx);
#if 0
  /* Register cluster commands handler for a specific endpoint */
  ZB_AF_SET_ENDPOINT_HANDLER(MY_ENDPOINT_NUMBER, zcl_specific_cluster_cmd_handler);
#endif
  /** [REGISTER] */

/* [zboss_main_loop_iteration_usage] */
  if (zboss_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zboss_start failed", (FMT__0));
  }
  else
  {
    zboss_main_loop();
  }
/* [zboss_main_loop_iteration_usage] */

  /* Deinitialize trace */
  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);

  switch(sig)
  {
  case ZB_ZDO_SIGNAL_SKIP_STARTUP:
    /* in case zboss was started with zboss_start_no_autostart(): without starting the commissioning */
    TRACE_MSG(TRACE_APP1, "ZB_ZDO_SIGNAL_SKIP_STARTUP: boot, not started yet", (FMT__0));
    if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
      zboss_start_continue();
    break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      TRACE_MSG(TRACE_APP1, "FIRST_START: start steering", (FMT__0));
      if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
      break;

    case ZB_COMMON_SIGNAL_CAN_SLEEP:
      /* for sleepy end device & low power */
#ifdef ZB_USE_SLEEP
      zb_sleep_now();
#endif
      break;

    default:
      break;
  }

  if (param)
  {
    zb_buf_free(param);
  }
}

/* Extra topics: use i2c:
 * MCXW71 has lpi2c1 where an accelerator is connected, refer to
 * zephyr/boards/nxp/frdm_mcxw71/frdm_mcxw71.dts, arduino_i2c
 * to use it on a zephyr application, see the sample:
 * zephyr/samples/boards/st/i2c_timing/src/main.c
 */


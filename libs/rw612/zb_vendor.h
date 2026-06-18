/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2023-2026 NXP
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
/* PURPOSE: ZBOSS stack configuration file for RW612 host build
*/

#ifndef ZB_VENDOR_CFG_RW612_HOST_H
#define ZB_VENDOR_CFG_RW612_HOST_H 1

/* NXP features */
//#define NXP_DUALPAN_SENSE_PAN_CHANNEL
//#define NXP_MACSPLIT_RESET_WITH_REASON


/* Memory options */
#define ZB_CONFIGURABLE_MEM /* MANDATORY */
#define ZB_VENDOR_MEM_CONFIG "zb_mem_config_rw612.h"
#if defined(ZB_CONFIGURABLE_MEM) && defined(ZB_CONFIG_DEFAULT_KERNEL_DEFINITION)
/* Default memory storage configuration - to be used if user does not include any of zb_mem_config_xxx.h */
#define ZB_CONFIG_OVERALL_NETWORK_SIZE 64
#define ZB_CONFIG_LIGHT_TRAFFIC
#define ZB_CONFIG_APPLICATION_SIMPLE
#endif  /*  ZB_CONFIG_DEFAULT_KERNEL_DEFINITION */


/* Zigbee configuration */
#define ZB_CONFIG_ZEPHYR_MACSPLIT_NXP_HOST
//#define ZB_MAC_TESTING_MODE


/* NVRAM configuration */


/* Sensitivity based on device spec: Need to be updated accordingly */
#define ZB_RSSI_MIN -102
#define ZB_RSSI_MAX 22
#define ZB_LQI_MIN 0
#define ZB_LQI_MAX 255


/* MACSPLIT and transport options */
//#define ZB_MACSPLIT_TRACE_DUMP_TO_FILE
//#define ZB_MACSPLIT_SPINEL
#define ZB_MACSPLIT_TRANSPORT_EXTERNAL
//#define ZB_MACSPLIT_ENABLE_BOOT_IND_WATCHDOG


/* Log level and log file management */
#define ZB_NXP_WCS_TRACE
#define ZB_ZBOSS_TRACE
#ifdef ZB_ZBOSS_TRACE
#define ZB_TRACE_LEVEL 4
#define ZB_TRACE_MASK  0x9D8 /* 0x800 + NWK APS ZDO SECUR ZCL */
#else
#define ZB_TRACE_LEVEL 0
#define ZB_TRACE_MASK  0
#endif
//#define ZB_USE_LOGFILE_ROTATE

#include "zb_cfg_macsplit_nxp.h"

#endif /* ZB_VENDOR_CFG_RW612_HOST_H */

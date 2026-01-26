/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2023-2025 NXP
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
/* PURPOSE: ZBOSS stack configuration file for Linux host build
*/

#ifndef ZB_VENDOR_CFG_LINUX_HOST_H
#define ZB_VENDOR_CFG_LINUX_HOST_H 1

/* NXP features */
#define NXP_DUALPAN_SENSE_PAN_CHANNEL
#define NXP_MACSPLIT_RESET_WITH_REASON


/* Memory options */
#define ZB_CONFIGURABLE_MEM /* MANDATORY */
#define ZB_VENDOR_MEM_CONFIG "zb_mem_config_iw61x.h"
#if defined(ZB_CONFIGURABLE_MEM) && defined(ZB_CONFIG_DEFAULT_KERNEL_DEFINITION)
/* Default memory storage configuration - to be used if user does not include any of zb_mem_config_xxx.h */
#define ZB_CONFIG_OVERALL_NETWORK_SIZE 250
#define ZB_CONFIG_HIGH_TRAFFIC
#define ZB_CONFIG_APPLICATION_COMPLEX
#endif  /*  ZB_CONFIG_DEFAULT_KERNEL_DEFINITION */


/* Zigbee configuration */
#define ZB_CONFIG_LINUX_MACSPLIT_HOST
#define ZB_FILE_PATH_MGMNT


/* NVRAM configuration */
#define ZB_NVRAM_PAGE_SIZE  0x10000


/* Sensitivity based on device spec */
#define ZB_RSSI_MIN -102
#define ZB_RSSI_MAX 22
#define ZB_LQI_MIN 0
#define ZB_LQI_MAX 255


/* MACSPLIT and transport options */
#define ZB_MACSPLIT_TRACE_DUMP_TO_FILE
#define ZB_MACSPLIT_SPINEL
#define ZB_MACSPLIT_TRANSPORT_SERIAL
//#define ZB_MACSPLIT_ENABLE_BOOT_IND_WATCHDOG /!\ Can not be used with CLI app, timeout is not correctly handled
#define ZB_TRANSPORT_LINUX_UART


/* Log level and log file management */
#define ZB_TRACE_LEVEL 4
#define ZB_TRACE_MASK  0xffffffff
#define ZB_USE_LOGFILE_ROTATE


/* Test and debug options */
#ifdef DEBUG
#define ZB_TRAFFIC_DUMP_ON
#endif /* DEBUG */


#include "zb_cfg_macsplit_nxp.h"

#endif /* ZB_VENDOR_CFG_LINUX_HOST_H */

/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
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
/* PURPOSE: ZBOSS stack configuration file for RPi host build
*/

#ifndef ZB_VENDOR_CFG_RPI_HOST_H
#define ZB_VENDOR_CFG_RPI_HOST_H 1

/* Configure trace by changing ZB_TRACE_MASK (one bit per layer)
 * and ZB_TRACE_LEVEL (messages with layer > ZB_TRACE_LEVEL are unvisible).
 * Do not define ZB_TRACE_LEVEL to switch off trace.
 */
#define ZB_TRACE_LEVEL 4
#define ZB_TRACE_MASK  0xffffffff

/* +++ zb_vendor_cfg_host_dualpan_spi.h */

/* +++ zb_vendor_cfg_host_dualpan_base.h */

/* +++ zb_vendor_cfg_host_base.h */

#define ZB_PIB_PTA_ATTRS

#define APS_FRAGMENTATION
#define ZB_ALL_DEVICE_SUPPORT
#define ZB_PRODUCTION_CONFIG
#define ZB_SECURITY_INSTALLCODES
#define ZB_MAC_CONFIGURABLE_TX_POWER
//#define ZB_MAC_TESTING_MODE
#define NXP_USE_NEIGHBOR_TABLE_PHY
#define NXP_EMPTY_FRAME_DISABLE
#define NXP_DUALPAN_SENSE_PAN_CHANNEL
#define NXP_HW_LQI_PRECISION_ENABLE 1
#define NXP_GET_TXPOWER_CAPABILITIES
#define NXP_GET_RX_SENSITIVITY
#define NXP_MACSPLIT_RESET_WITH_REASON

#define ZB_CONFIGURABLE_MEM /* MANDATORY */

#define ZB_VENDOR_MEM_CONFIG "zb_mem_config_iw61x.h"
/* +++  */
/* ---  */
#if defined(ZB_CONFIGURABLE_MEM) && defined(ZB_CONFIG_DEFAULT_KERNEL_DEFINITION)
/* Default memory storage configuration - to be used if user does not include any of zb_mem_config_xxx.h */
#ifndef ZB_ED_ROLE
#define ZB_CONFIG_ROLE_ZC
#else
#define ZB_CONFIG_ROLE_ZED
#endif

#define ZB_CONFIG_OVERALL_NETWORK_SIZE 250

#define ZB_CONFIG_HIGH_TRAFFIC

#define ZB_CONFIG_APPLICATION_COMPLEX

#endif  /*  ZB_CONFIG_DEFAULT_KERNEL_DEFINITION */


/* Compile-time memory configuration: hard-coded parameters */

/* Set MAX ENDPOINTS to 20 */
#define ZB_MAX_EP_NUMBER 20
/* Enable & configure DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE, max ZB_MAX_EP_NUMBER */
#define ZB_ZCL_ALLOW_DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE
/* Enable & configure FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER, max ZB_MAX_EP_NUMBER */
#define ZB_ZCL_ALLOW_FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER

/* FIXME: 32 seems too big for zb_mlme_scan_confirm() - stack is allocating 5 + (5 *
 * ZB_PANID_TABLE_SIZE) in buf. So maximum possible ZB_PANID_TABLE_SIZE is 28 */
#define ZB_PANID_TABLE_SIZE 28
#define ZB_DEV_MANUFACTORER_TABLE_SIZE 32
#define ZB_BUF_Q_SIZE 64
#define ZDO_TRAN_TABLE_SIZE 128
#define ZB_APS_ENDPOINTS_IN_GROUP_TABLE 8

#define ZB_APS_GROUP_TABLE_SIZE 16
#define ZB_ENABLE_ZGP
#define ZB_ZGP_SINK_TBL_SIZE 24
#define ZB_ZGP_PROXY_TBL_SIZE 5
#define ZB_ZGP_TRANSL_CMD_PLD_MAX_SIZE 3
#define ZB_BDB_MODE
#define ZB_BDB_ENABLE_FINDING_BINDING
#define ZB_DISTRIBUTED_SECURITY_ON
#define ZB_SECURITY_INSTALLCODES

#define ZB_MAX_ED_CAPACITY_DEFAULT 64

#define ZB_MAC_INTERFACE_SINGLE

#define ZB_RSSI_MIN -102
#define ZB_RSSI_MAX 22
#define ZB_LQI_MIN 0
#define ZB_LQI_MAX 255

#ifdef DEBUG
#define ZB_DEBUG_BUFFERS
#define ZB_TRAFFIC_DUMP_ON
#define ZB_CHECK_OOM_STATUS
#define ZB_SEND_OOM_STATUS
#if !defined(USE_ASSERT)
#define USE_ASSERT
#endif /* !USE_ASSERT */
#endif /* DEBUG */


#define ZB_FILE_PATH_MGMNT

#define ZB_NVRAM_PAGE_SIZE  0x10000

#ifndef ZB_NVRAM_BUF_SIZE
#define ZB_NVRAM_BUF_SIZE   256
#endif

#define ZB_THREADS
#define ZB_ZBOSS_DEINIT

/* ZB_INTERRUPT_SAFE_xxx  Is necessary to allocate buffers and schedule
callbacks from the thread other than the main ZBOSS thread on Unix-like
systems (cf email: Stepan 08/12/2023, 10:54)*/
#define ZB_INTERRUPT_SAFE_CALLBACKS
/* Do not activate ZB_INTERRUPT_SAFE_ALARMS because several tests such as (TP_PED-1, ) are failing */
/* #define ZB_INTERRUPT_SAFE_ALARMS */
#define ZB_INTERRUPT_SAFE_BUFS

#if defined __GNUC__ && defined __x86_64__
#undef ZB_TRACE_USE_FULL_BUFFERING
#else
#define ZB_TRACE_USE_FULL_BUFFERING
#endif

//#define ZB_DISABLE_SIGTERM_HANDLER
#define ZB_SIGTERM_EXIT_GRACEFULLY

#define ZB_TRACE_PRINT_SHORT_FILE_NAME

#define ZB_DEFAULTS_GENERATE_RANDOM_EXT_ADDRESS

/* we hope it will help with response times in busy networks */
#define ZB_ENABLE_NWK_RETRANSMIT

#define ZB_REDUCE_NWK_LOAD_ON_LOW_MEMORY

/*
 * NWK configuration
 */
//#define ZB_NWK_ROUTE_DISCOVERY_TABLE_SIZE
#define ZB_NWK_PENDING_TABLE_SIZE 20
#define ZB_NWK_BTR_TABLE_SIZE 32
#define ZB_NWK_BRR_TABLE_SIZE 16
//#define ZB_NWK_REJOIN_REQUEST_TABLE_SIZE 1U
//#define ZB_NWK_IMMEDIATE_REJOIN_DELAY_MS (1000U)
//#define ZB_NWK_CHANNEL_ACCEPT_LEVEL 0x60U
//#define ZB_NWK_NEIGHBOR_RSSI_FILTER (-80)
//#define ZB_DEFAULT_PERMIT_JOINING_DURATION 0xffU

/*
 * MAC configuration
 * NOT USED IN MACSPLIT ARCHITECTURE
 */
//#define ZB_MAC_QUEUE_SIZE 8U
//#define ZB_DONT_SET_DEFAULT_IEEE_ADDRESS
//#define ZB_SEND_BEACON_IMMEDIATELY
//#define ZB_MAC_HANDLE_BEACON_REQ_HI_TMO
//#define ZB_MAC_HANDLE_BEACON_REQ_LOW_TMO
//#define ZB_MAC_TESTING_MODE
//#define ZB_MAC_DUTY_CYCLE_RAMP_UP_SYMBOLS 0U
//#define ZB_MAC_DUTY_CYCLE_RAMP_DOWN_SYMBOLS 0U
//#define ZB_USE_DUTY_CYCLE_PERCENT_ENABLE
//#define ZB_MAC_DEFAULT_TX_POWER_GB_EU_SUB_GHZ +14
//#define ZB_MAC_DEFAULT_TX_POWER_NA_SUB_GHZ +30
//#define ZB_MAC_DEFAULT_TX_POWER_24_GHZ  +20
//#define ZB_MAC_ASSOCIATION_DATA_REQUEST_COUNT 1U
//#define ZB_ZGP_MAX_PAIRED_CONF_GPD_COMMANDS 16U

/* ----------------- WWAH ---------------------- */
#define ZB_ZCL_SUPPORT_CLUSTER_WWAH
#define ZB_ZCL_ENABLE_WWAH_SERVER
#define ZB_PARENT_CLASSIFICATION

#define ZB_LITE_NO_ZB_DIRECT
#define ZB_MACSPLIT_TRANSPORT_IGNORE_UNTIL_DEVICE_BOOT_RECEIVED

/* --- zb_vendor_cfg_host_base.h */

#define ZB_CONFIG_LINUX_MACSPLIT_HOST

#define ZB_HAVE_SERIAL

#define SUPPORT_GPIO_DEV_GPIOCHIP

/* Reduce MCPS Poll Indication calls from MAC layer */
#define ZB_MAC_POLL_INDICATION_CALLS_REDUCED

#define ZB_MACSPLIT_TRACE_DUMP_TO_FILE

#define ZB_MACSPLIT_SPINEL

#define ZB_MACSPLIT_TRANSPORT_SERIAL
#define ZB_TRANSPORT_LINUX_UART

#define SERIAL_ENV_NAME "MUX_ZB_TTY"
#define SERIAL_DEFAULT_PATH "/tmp/ttyMACSPL"

/* We want to get IEEE address from HW and do not set it randomly in zb_set_default_mac_addr() */
#define ZB_DONT_SET_DEFAULT_IEEE_ADDRESS

#define ZB_MACSPLIT_NO_ACK_DELAY

#define ZB_ENABLE_PTA

/* TODO: make that switches OFF by default */
#define ZB_LITE_NO_ZB_DIRECT
#define ZB_MAC_INTERFACE_SINGLE

/* --- zb_vendor_cfg_host_dualpan_base.h */

/* no additional defines are required because host communicates with SoC via muxer which provides the same interface for
   both SPI and UART transports.
   Muxer supports both transports regardless of defines. */

/* --- zb_vendor_cfg_host_dualpan_spi.h */

#define TC_SWAPOUT
#if 0 /* Flag for SubGhz which is not supported by IW612 */
#define ZB_ENHANCED_BEACON_SUPPORT
#endif
#define ZB_CERTIFICATION_HACKS
#define ZB_STACK_REGRESSION_TESTING_API
//#define ZB_NSNG_CI
//#define ZB_R22_MULTIMAC_MODE
#define ZB_MGMT_NWK_ENHANCED_UPDATE_ENABLED
#define ZB_MULTITEST_USE_STDIO
#define ZB_LIMIT_VISIBILITY
#define ZB_CONFIGURABLE_RETRIES

#define ZB_USE_LOGFILE_ROTATE

#ifdef ZB_USE_LOGFILE_ROTATE
#define LOG_ROTATE_1M 1024*1024
#define ZB_DEFAULT_MAX_LOGFILE_SIZE   (50 * LOG_ROTATE_1M)
//#define ZB_DEFAULT_TOTAL_LOGS_SIZE    (150 * LOG_ROTATE_1M)
#define MAX_LOGFILE_INDEX  3 /* so max size: total ZB_DEFAULT_MAX_LOGFILE_SIZE * MAX_LOGFILE_INDEX */
#define ZB_LOGFILE_POST_ROTATE_SH  "ZB_LOGFILE_POST_ROTATE_SH"
#endif  /* ZB_USE_LOGFILE_ROTATE */

#define ZB_MUX_STORE_LAST_MACSPLIT_MSGS_NUMBERS 32

#define ZB_RESET_AUTORESTART	/* Required by zb_reset() */

#define ZB_MACSPLIT_RESET_DEVICE_AT_START

#define ZB_MACSPLIT_TX_WIN_SIZE 2u

#define ZBOSS_ZDO_APP_TSN_ENABLE
#define ZBOSS_ZDO_APP_TSN_TABLE_SIZE 128

#endif /* ZB_VENDOR_CFG_RPI_HOST_H */

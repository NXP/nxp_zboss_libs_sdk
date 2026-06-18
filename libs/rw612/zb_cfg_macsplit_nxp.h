/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA.
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
/* PURPOSE: ZBOSS stack common configuration file for NXP platforms
*/

#ifndef ZB_VENDOR_CFG_NXP_COMMON_H
#define ZB_VENDOR_CFG_NXP_COMMON_H 1

/* NXP features */
#define NXP_USE_NEIGHBOR_TABLE_PHY
#define NXP_EMPTY_FRAME_DISABLE
#define NXP_HW_LQI_PRECISION_ENABLE 1
#define NXP_GET_TXPOWER_CAPABILITIES
#define NXP_GET_RX_SENSITIVITY
#define ZB_MUX_STORE_LAST_MACSPLIT_MSGS_NUMBERS 32


/* Memory options */
#if defined(ZB_CONFIGURABLE_MEM) && defined(ZB_CONFIG_DEFAULT_KERNEL_DEFINITION)
/* Default memory storage configuration - to be used if user does not include any of zb_mem_config_xxx.h */
#ifndef ZB_ED_ROLE
#define ZB_CONFIG_ROLE_ZC
#else
#define ZB_CONFIG_ROLE_ZED
#endif
#endif  /*  ZB_CONFIG_DEFAULT_KERNEL_DEFINITION */


/* Zigbee configuration */
#define ZB_ALL_DEVICE_SUPPORT
#define SUPPORT_GPIO_DEV_GPIOCHIP
#define ZB_PRODUCTION_CONFIG
#define ZB_MAC_CONFIGURABLE_TX_POWER
#define TC_SWAPOUT
#define ZB_MAX_EP_NUMBER 20
#define ZB_MAX_ED_CAPACITY_DEFAULT 64
#define ZB_BDB_MODE
#define ZB_BDB_ENABLE_FINDING_BINDING
#define ZB_DISTRIBUTED_SECURITY_ON
#define ZB_SECURITY_INSTALLCODES
#define ZB_DEFAULTS_GENERATE_RANDOM_EXT_ADDRESS
#define ZB_PANID_TABLE_SIZE 28
#define ZB_RESET_AUTORESTART	/* Required by zb_reset() */
#define ZB_ZBOSS_DEINIT
//#define ZB_DISABLE_SIGTERM_HANDLER
#define ZB_SIGTERM_EXIT_GRACEFULLY


/* APS configuration */
#define APS_FRAGMENTATION
#define ZB_APS_ENDPOINTS_IN_GROUP_TABLE 8
#define ZB_APS_GROUP_TABLE_SIZE 16


/* Manufacturer profile enablement */
#define ZB_ZCL_ALLOW_DYNAMIC_MANUFACTURER_SPECIFIC_PROFILE
#define ZB_ZCL_ALLOW_FRAGMENTATION_ON_MANUFACTURER_SPECIFIC_CLUSTER


/* Scheduler and queue configuration */
#define ZB_CB_QUANT 8 //Max number of cbs to be executed at each scheduler loop
#define ZB_BUF_Q_SIZE 64 //Size of queue for wait for free packet buffer


/* Green power configuration */
#define ZB_ENABLE_ZGP
#define ZB_ZGP_SINK_TBL_SIZE 24
#define ZB_ZGP_PROXY_TBL_SIZE 5
#define ZB_ZGP_TRANSL_CMD_PLD_MAX_SIZE 3


/* NVRAM configuration */
#ifndef ZB_NVRAM_BUF_SIZE
#define ZB_NVRAM_BUF_SIZE   256
#endif


/* Interrupt management: Required in multi-threading */
#define ZB_THREADS
#define ZB_INTERRUPT_SAFE_CALLBACKS
/* Do not activate ZB_INTERRUPT_SAFE_ALARMS because several tests such as (TP_PED-1, ) are failing */
/* #define ZB_INTERRUPT_SAFE_ALARMS */
#define ZB_INTERRUPT_SAFE_BUFS


/* NWK configuration */
#define ZB_NWK_PENDING_TABLE_SIZE 20
#define ZB_NWK_BTR_TABLE_SIZE 32
#define ZB_NWK_BRR_TABLE_SIZE 16
#define ZB_REDUCE_NWK_LOAD_ON_LOW_MEMORY
//#define ZB_NWK_REJOIN_REQUEST_TABLE_SIZE 1U
//#define ZB_NWK_IMMEDIATE_REJOIN_DELAY_MS (1000U)
//#define ZB_NWK_CHANNEL_ACCEPT_LEVEL 0x60U
//#define ZB_NWK_NEIGHBOR_RSSI_FILTER (-80)
//#define ZB_DEFAULT_PERMIT_JOINING_DURATION 0xffU

/*  WWAH support */
#define ZB_ZCL_SUPPORT_CLUSTER_WWAH
#define ZB_ZCL_ENABLE_WWAH_SERVER
#define ZB_PARENT_CLASSIFICATION

/* OTA support */
#define ZB_USE_OSIF_OTA_ROUTINES

/* TSN support */
#define ZBOSS_ZDO_APP_TSN_ENABLE
#define ZBOSS_ZDO_APP_TSN_TABLE_SIZE 128
#define ZDO_TRAN_TABLE_SIZE 128


/* MACSPLIT and transport options */
#define ZB_MAC_INTERFACE_SINGLE

#define ZB_MACSPLIT_RESET_DEVICE_AT_START
#define ZB_MACSPLIT_TX_WIN_SIZE 2u
#define ZB_MACSPLIT_TRANSPORT_IGNORE_UNTIL_DEVICE_BOOT_RECEIVED

#define ZB_HAVE_SERIAL
#define SERIAL_ENV_NAME "MUX_ZB_TTY"
#define SERIAL_DEFAULT_PATH "/tmp/ttyMACSPL"


/* Log level and log file management */
#ifdef ZB_USE_LOGFILE_ROTATE
#define LOG_ROTATE_1M 1024*1024
#define ZB_DEFAULT_MAX_LOGFILE_SIZE   (50 * LOG_ROTATE_1M)
//#define ZB_DEFAULT_TOTAL_LOGS_SIZE ZB_TRACE_PRINT_SHORT_FILE_NAME   (150 * LOG_ROTATE_1M)
#define MAX_LOGFILE_INDEX  3 /* so max size: total ZB_DEFAULT_MAX_LOGFILE_SIZE * MAX_LOGFILE_INDEX */
#define ZB_LOGFILE_POST_ROTATE_SH  "ZB_LOGFILE_POST_ROTATE_SH"
#endif  /* ZB_USE_LOGFILE_ROTATE */

#define ZB_TRACE_PRINT_SHORT_FILE_NAME

#if defined __GNUC__ && defined __x86_64__
#undef ZB_TRACE_USE_FULL_BUFFERING
#else
#endif


/* Test and debug options */
#define ZB_CERTIFICATION_HACKS
#define ZB_STACK_REGRESSION_TESTING_API

#define ZB_LIMIT_VISIBILITY
#define ZB_CONFIGURABLE_RETRIES

#ifdef DEBUG
#define ZB_DEBUG_BUFFERS
#define ZB_CHECK_OOM_STATUS
#define ZB_SEND_OOM_STATUS
#if !defined(USE_ASSERT)
#define USE_ASSERT
#endif /* !USE_ASSERT */
#endif /* DEBUG */

#endif /* ZB_VENDOR_CFG_NXP_COMMON_H */

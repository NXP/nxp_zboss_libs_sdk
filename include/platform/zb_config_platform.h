/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2024-2025 NXP
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
/* PURPOSE: Configuration file: configuration- and platform-specific definitions,
constants etc.
*/
#ifndef ZB_PLATFORM_CONFIG_H
#define ZB_PLATFORM_CONFIG_H 1

/* General Linux platform definitions */
#ifndef __ZEPHYR__
#define __ZEPHYR__
#endif
#define ZB_ENABLE_ZCL
#define ZB_LITTLE_ENDIAN
#define ZB_NEED_ALIGN
#ifndef ZB_PLATFORM_ZEPHYR
#define ZB_PLATFORM_ZEPHYR
#endif

#if defined(__APPLE__) && defined(__MACH__)
#ifndef MACOSX
#define MACOSX
#endif /* MACOSX */
#endif /* __APPLE__ && __MACH__ */

#ifndef ZB_ZGPD_GPFS_SIZE
/* Don't waste traffic, but jow we can send more than one */
#define ZB_ZGPD_GPFS_SIZE 2
#endif
#ifndef ZB_ZGP_TRANSL_CMD_PLD_MAX_SIZE
#define ZB_ZGP_TRANSL_CMD_PLD_MAX_SIZE 3
#endif
#define GPDF_TX_DELAY_DECREASE 200

#ifndef ZB_CONFIGURABLE_MEM
#define ZB_CHILD_HASH_TABLE_SIZE (((ZB_IEEE_ADDR_TABLE_SIZE + ZB_IEEE_ADDR_TABLE_SIZE / 3) + 31) / 32 * 32)
#endif /* ZB_CONFIGURABLE_MEM */

#define MAC_TRANSPORT_USES_SELECT
#define ZB_MAC_PENDING_BIT_SOURCE_MATCHING
#define ZB_SOFT_SECURITY
#define ZB_DEBUG_ENLARGE_TIMEOUT 1

#ifndef ZB_CB_QUANT
#define ZB_CB_QUANT 2
#endif

#ifndef NCP_TRANSPORT_REFRESH_TIME
#define NCP_TRANSPORT_REFRESH_TIME (-1) /* Lock for an infinite amount of time on the transport select if no callbacks and alarms are scheduled. */
#endif /* NCP_TRANSPORT_REFRESH_TIME */

#ifdef ZB_CONFIG_LINUX_MACSPLIT_HOST
  #define ZB_MACSPLIT
  #define ZB_MACSPLIT_HOST
  #define ZB_MACSPLIT_FW_UPGRADE
#endif /* ZB_CONFIG_LINUX_MACSPLIT_HOST */
#ifdef ZB_CONFIG_ZEPHYR_MACSPLIT_NXP_HOST
  #define ZB_MACSPLIT
  #define ZB_MACSPLIT_HOST
#endif /* ZB_CONFIG_LINUX_MACSPLIT_HOST */
#ifdef ZB_MACSPLIT_HOST
  #define MAC_AUTO_DELAY_IN_MAC_GP_SEND

  #if defined ZB_MAC_INTERFACE_SINGLE || !defined ZB_MAC_MONOLITHIC
    #define ZB_ALIEN_MAC /* TODO: remove this define completely, use ZB_COMPILE_MAC_MONOLITHIC instead */
  #endif /* ZB_MAC_INTERFACE_SINGLE || !ZB_MAC_MONOLITHIC */
#endif   /* ZB_MACSPLIT_HOST */

/* trace */
#ifdef ZB_TRACE_LEVEL
#define ZB_TRACE_OVER_USART
#define ZB_TRACE_TO_PORT
#define ZB_BINARY_TRACE
#define ZB_HAVE_SERIAL
#define ZB_TRACE_SERIAL_BAUDRATE 115200U
#endif  /* if trace */


#endif  /* ZB_PLATFORM_CONFIG_H */

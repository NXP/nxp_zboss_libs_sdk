/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2025 NXP
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
/* PURPOSE: Custom memconfig file
*/

#ifndef ZB_MEM_CONFIG_CUSTOM_H
#define ZB_MEM_CONFIG_CUSTOM_H 1

#ifndef ZB_ED_ROLE
#define ZB_CONFIG_ROLE_ZC
#define ZB_CONFIG_OVERALL_NETWORK_SIZE 250
#define ZB_CONFIG_HIGH_TRAFFIC
#define ZB_CONFIG_APPLICATION_COMPLEX
#else
#define ZB_CONFIG_ROLE_ZED
#define ZB_CONFIG_OVERALL_NETWORK_SIZE 250
#define ZB_CONFIG_LIGHT_TRAFFIC
#define ZB_CONFIG_APPLICATION_COMPLEX
#endif

#include "zb_mem_config_common.h"

/* #undef ZB_CONFIG_IEEE_ADDR_TABLE_SIZE : already defined in stack with following */
//define ZB_CONFIG_IEEE_ADDR_TABLE_SIZE (ZB_CONFIG_OVERALL_NETWORK_SIZE + ZB_IEEE_ADDR_TABLE_SIZE_RESERVE(ZB_CONFIG_OVERALL_NETWORK_SIZE))

#undef ZB_CONFIG_IOBUF_POOL_SIZE /* high traffic config: 48 */
#define ZB_CONFIG_IOBUF_POOL_SIZE 240

/* #undef ZB_CONFIG_APS_BIND_TRANS_TABLE_SIZE : already defined in stack with following */
//#define ZB_CONFIG_APS_BIND_TRANS_TABLE_SIZE ((ZB_CONFIG_IOBUF_POOL_SIZE + 15U)/16U * 4U) //1/4, at least 4

#undef ZB_CONFIG_APS_SRC_BINDING_TABLE_SIZE /* high traffic config: 32*/
#define ZB_CONFIG_APS_SRC_BINDING_TABLE_SIZE 64

#undef ZB_CONFIG_APS_DST_BINDING_TABLE_SIZE /* high traffic config: 32*/
#define ZB_CONFIG_APS_DST_BINDING_TABLE_SIZE 64

#undef ZB_CONFIG_APS_DUPS_TABLE_SIZE /* high traffic config: 32*/
#define ZB_CONFIG_APS_DUPS_TABLE_SIZE 200

/* #undef ZB_CONFIG_N_APS_KEY_PAIR_ARR_MAX_SIZE : already defined in stack with following */
//#define ZB_CONFIG_N_APS_KEY_PAIR_ARR_MAX_SIZE ZB_CONFIG_OVERALL_NETWORK_SIZE

/* #undef ZB_CONFIG_N_APS_RETRANS_ENTRIES : already defined in stack with following */
//#define ZB_CONFIG_N_APS_RETRANS_ENTRIES ((ZB_CONFIG_IOBUF_POOL_SIZE + 8U)/9U * 3U) /* 1/3, at least 3 */

/* #undef ZB_CONFIG_NWK_DISC_TABLE_SIZE : already defined in stack with following */
//#define ZB_CONFIG_NWK_DISC_TABLE_SIZE ZB_CONFIG_OVERALL_NETWORK_SIZE

#undef ZB_CONFIG_NWK_ROUTE_DISC_TABLE_SIZE /* high traffic config: 32 */
#define ZB_CONFIG_NWK_ROUTE_DISC_TABLE_SIZE 64

/* #undef ZB_CONFIG_NEIGHBOR_TABLE_SIZE : already defined in stack with following */
//#define ZB_CONFIG_NEIGHBOR_TABLE_SIZE ZB_CONFIG_OVERALL_NETWORK_SIZE

/* #undef ZB_CONFIG_NWK_ROUTING_TABLE_SIZE : already defined in stack with following */
//#define ZB_CONFIG_NWK_ROUTING_TABLE_SIZE ZB_CONFIG_NEIGHBOR_TABLE_SIZE

#undef ZB_CONFIG_SCHEDULER_Q_SIZE /* high traffic config: 48 */
#define ZB_CONFIG_SCHEDULER_Q_SIZE 250

/* #undef ZB_CONFIG_MAC_PENDING_QUEUE_SIZE : already defined in stack with following */
//#define ZB_CONFIG_MAC_PENDING_QUEUE_SIZE (ZB_CONFIG_IOBUF_POOL_SIZE / 2U)

/* #undef ZB_CONFIG_ZDO_KEY_NEGOTIATIONS_NUM : already defined in stack with following */
//#define ZB_CONFIG_ZDO_KEY_NEGOTIATIONS_NUM (ZB_CONFIG_OVERALL_NETWORK_SIZE < 10u ? ZB_CONFIG_OVERALL_NETWORK_SIZE : 10u)

/* #undef ZB_CONFIG_NWK_MAX_SOURCE_ROUTES : already defined in stack with following */
//#define ZB_CONFIG_NWK_MAX_SOURCE_ROUTES ZB_CONFIG_OVERALL_NETWORK_SIZE

/* #undef ZB_CONFIG_SCHEDULER_Q_SIZE_PROTECTED_STACK_POOL : already defined in stack with following */
//#define ZB_CONFIG_SCHEDULER_Q_SIZE_PROTECTED_STACK_POOL 12U

/* #undef ZB_CONFIG_SINGLE_TRANS_INDEX_SIZE : already defined in stack with following */
//#define ZB_CONFIG_SINGLE_TRANS_INDEX_SIZE ((ZB_CONFIG_APS_BIND_TRANS_TABLE_SIZE + 7U) / 8U)

/* #undef ZB_CONFIG_BUF_POOL_BITMAP_SIZE : already defined in stack with following */
//#define ZB_CONFIG_BUF_POOL_BITMAP_SIZE ((ZB_CONFIG_IOBUF_POOL_SIZE + 7U) / 8U)

#include "zb_mem_config_context.h"


#endif // ZB_MEM_CONFIG_CUSTOM_H

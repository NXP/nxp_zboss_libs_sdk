/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2024 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
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
/* PURPOSE: ZCL separate endpoint settings for devices in ZC role (BDB mode)
*/

#ifndef LIGHT_ZC_CLUSTERS_H
#define LIGHT_ZC_CLUSTERS_H

#define LIGHT_ZC_DEVICE_ID   0xaa01
#define LIGHT_ZC_DEVICE_VER  0

#define LIGHT_ZC_IN_CLUSTER_NUM 2
#define LIGHT_ZC_OUT_CLUSTER_NUM 1


#define LIGHT_ZC_CLUSTER_NUM (                       \
                         (LIGHT_ZC_IN_CLUSTER_NUM) + \
                         (LIGHT_ZC_OUT_CLUSTER_NUM))


#define LIGHT_ZC_DECLARE_CLUSTER_LIST(                            \
  cluster_list_name,                                              \
  basic_attr_list,                                                \
  keep_alive_attr_list,                                           \
  poll_ctrl_attr_list)                                            \
  zb_zcl_cluster_desc_t cluster_list_name[] =                     \
  {                                                               \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                    \
      ZB_ZCL_ARRAY_SIZE((basic_attr_list), zb_zcl_attr_t),        \
      (basic_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                               \
      ZB_ZCL_ARRAY_SIZE((keep_alive_attr_list), zb_zcl_attr_t),   \
      (keep_alive_attr_list),                                     \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                               \
      ZB_ZCL_ARRAY_SIZE((poll_ctrl_attr_list), zb_zcl_attr_t),    \
      (poll_ctrl_attr_list),                                      \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    )                                                             \
  }

#define LIGHT_ZC_DECLARE_SIMPLE_DESC(                                          \
  ep_name, ep_id, in_clust_num, out_clust_num)                                 \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =  \
  {                                                                            \
    (ep_id),                                                                   \
    ZB_AF_HA_PROFILE_ID,                                                       \
    LIGHT_ZC_DEVICE_ID,                                                        \
    LIGHT_ZC_DEVICE_VER,                                                       \
    0,                                                                         \
    in_clust_num,                                                              \
    out_clust_num,                                                             \
    {                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                            \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL                                           \
    }                                                                          \
  }


#define LIGHT_ZC_DECLARE_EP(                                            \
  ep_name, ep_id, cluster_list)                                         \
  LIGHT_ZC_DECLARE_SIMPLE_DESC(                                         \
    ep_name,                                                            \
    (ep_id),                                                            \
    LIGHT_ZC_IN_CLUSTER_NUM,                                            \
    LIGHT_ZC_OUT_CLUSTER_NUM);                                          \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
    ep_name,                                                            \
    (ep_id),                                                            \
    ZB_AF_HA_PROFILE_ID,                                                \
    0,                                                                  \
    NULL,                                                               \
    ZB_ZCL_ARRAY_SIZE((cluster_list), zb_zcl_cluster_desc_t),           \
    (cluster_list),                                                     \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                   \
    0, NULL,                                                            \
    0, NULL)

#endif /* LIGHT_ZC_CLUSTERS_H */
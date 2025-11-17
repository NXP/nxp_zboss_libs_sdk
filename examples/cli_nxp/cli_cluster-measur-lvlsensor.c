/*
 * Copyright 2025 NXP
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


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Illuminance Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Illuminance Level Sensing
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Temperature Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* variable hidden  in macro ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION */
static zb_uint16_t cluster_revision_temp_measurement_attr_list = ZB_ZCL_TEMP_MEASUREMENT_CLUSTER_REVISION_DEFAULT;

/* This table is extracted from macro
 *   ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(temp_measurement_attr_list, 
 *                                               &g_attr_temp_measurement_value, 
 *                                               &g_attr_temp_measurement_min_value, 
 *                                               &g_attr_temp_measurement_max_value, 
 *                                               &g_attr_temp_measurement_tolerance);
 * to have a better view of its content
 *     ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_TEMP_MEASUREMENT)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, &g_attr_temp_measurement_value)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, &g_attr_temp_measurement_min_value)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, &g_attr_temp_measurement_max_value)
 *     ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID, &g_attr_temp_measurement_tolerance)
 *     ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
 */

static zb_zcl_attr_t cluster_attr_0402[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,    ZB_ZCL_ATTR_TYPE_U16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&cluster_revision_temp_measurement_attr_list },
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,     ZB_ZCL_ATTR_TYPE_S16, ACC_READ_ONLY | ACC_REPORTING, NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.value },
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, ZB_ZCL_ATTR_TYPE_S16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.min_value },
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, ZB_ZCL_ATTR_TYPE_S16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.max_value },
#ifndef ZB_DISABLE_TEMP_MEASUREMENT_TOLERANCE_ID
  { ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID, ZB_ZCL_ATTR_TYPE_U16, ACC_READ_ONLY,                 NO_MANUF_SPE, (void*)&g_lvlsensor_temp_measurement_attr.tolerance },
#endif
  /* Optional attributes */
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             NO_MANUF_SPE, NULL }
};


static zb_cluster_def cluster_0402 = {
  cluster_attr_0402,
  sizeof(cluster_attr_0402)/sizeof(zb_zcl_attr_t),
  zb_zcl_temp_measurement_init_server,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_temp_measurement_init_client,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  dummy_commands_handler,
};


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Pressure Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Flow Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Relative Humidity Measurement
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Occupancy Sensing
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Electrical Measurement
 *
 * ----------------------------------------------------------------------------------- */




/*
 * Copyright 2024-2026 NXP
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


#ifdef CLI_HAS_CLUSTER_BASIC
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Basic
 *
 * ----------------------------------------------------------------------------------- */

/* Basic cluster attributes */
typedef struct {
  /* ZCL version attribute */
  zb_uint8_t zcl_version;
  /* Power source attribute */
  zb_uint8_t power_source;
} zb_zcl_general_basic_attr_t;

static zb_zcl_general_basic_attr_t g_general_basic_attr = {
  .zcl_version  = ZB_ZCL_VERSION,
  .power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN,
};
#endif /* CLI_HAS_CLUSTER_BASIC */

/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Power Configuration
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Device Temperature Configuration
 *
 * ----------------------------------------------------------------------------------- */


#ifdef CLI_HAS_CLUSTER_IDENTITY
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Identify
 *
 * ----------------------------------------------------------------------------------- */

/* Identify cluster attributes */
typedef struct {
  zb_uint16_t identify_time;
} zb_zcl_general_identity_attr_t;

static zb_zcl_general_identity_attr_t g_general_identity_attr = {
  .identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,
};
#endif /* CLI_HAS_CLUSTER_IDENTITY */


#ifdef CLI_HAS_CLUSTER_GROUPS
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Groups
 *
 * ----------------------------------------------------------------------------------- */

/* Groups cluster attributes */
typedef struct {
  zb_uint8_t name_support;
} zb_zcl_general_groups_attr_t;

static zb_zcl_general_groups_attr_t g_general_groups_attr = {
  .name_support = 0,
};
#endif /* CLI_HAS_CLUSTER_GROUPS */


#ifdef CLI_HAS_CLUSTER_SCENES
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Scenes
 *
 * ----------------------------------------------------------------------------------- */

/* Scenes cluster attributes */
typedef struct {
  zb_uint8_t  scene_count;
  zb_uint8_t  current_scene;
  zb_uint16_t current_group;
  zb_uint8_t  scene_valid;
  zb_uint16_t name_support;
} zb_zcl_general_scenes_attr_t;

static zb_zcl_general_scenes_attr_t g_general_scenes_attr = {
  .scene_count   = ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE,
  .current_scene = ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE,
  .current_group = ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE,
  .scene_valid   = ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE,
  .name_support  = ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE,
};
#endif /* CLI_HAS_CLUSTER_SCENES */


#ifdef CLI_HAS_CLUSTER_ONOFF
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER ON/OFF
 *
 * ----------------------------------------------------------------------------------- */


/* On/Off cluster attributes */
typedef struct {
  zb_uint8_t on_off;
} zb_zcl_general_on_off_attr_t;

static zb_zcl_general_on_off_attr_t g_general_on_off_attr = {
  .on_off = ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE,
};
#endif /* CLI_HAS_CLUSTER_ONOFF */


#ifdef CLI_HAS_CLUSTER_ONOFF_SWITCH
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER ON/OFF Switch Configuration
 *
 * ----------------------------------------------------------------------------------- */

/* Switch config cluster attributes */
typedef struct {
  zb_uint8_t type;
  zb_uint8_t actions;
} zb_zcl_general_on_off_switch_attr_t;

static zb_zcl_general_on_off_switch_attr_t g_general_on_off_switch_attr = {
  .type    = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_TOGGLE,
  .actions = ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_DEFAULT_VALUE,
};
#endif /* CLI_HAS_CLUSTER_ONOFF_SWITCH */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Level Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Alarms
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Time
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER RSSI Location
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Analog Input
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Analog Output
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Analogue Value
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Binary Input
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Binary Output
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Binary Value
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Multistate Input
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Multistate Output
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Multistate Value
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Diagnostics
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Poll Control
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Power Profile
 *
 * ----------------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Meter Identification
 *
 * ----------------------------------------------------------------------------------- */


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


#ifdef CLI_HAS_CLUSTER_TEMPERATURE
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Temperature Measurement
 *
 * ----------------------------------------------------------------------------------- */

/* On/Off cluster attributes */
typedef struct {
  zb_int16_t value;
  zb_int16_t min_value;
  zb_int16_t max_value;
  zb_uint16_t tolerance;
} zb_zcl_measur_lvlsensor_temp_measurement_attr_t;

static zb_zcl_measur_lvlsensor_temp_measurement_attr_t g_lvlsensor_temp_measurement_attr = {
  .value = ZB_ZCL_TEMP_MEASUREMENT_VALUE_DEFAULT_VALUE,
  .min_value = ZB_ZCL_TEMP_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE,
  .max_value = ZB_ZCL_TEMP_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE,
#ifndef ZB_DISABLE_TEMP_MEASUREMENT_TOLERANCE_ID
  .tolerance = 0,
#endif
};
#endif /* CLI_HAS_CLUSTER_TEMPERATURE */


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


#if defined CLI_HAS_CLUSTER_OTA_UPGRADE_SRV || defined CLI_HAS_CLUSTER_OTA_UPGRADE_CLT
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER OTA Upgrade
 *
 * ----------------------------------------------------------------------------------- */

/* Client config */
#define OTA_UPGRADE_DEFAULT_MANUFACTURER       0xFFFF
#define OTA_UPGRADE_DEFAULT_IMAGE_TYPE         0xFFFF

/* OTA Upgrade client cluster attributes */
typedef struct {
  /* UpgradeServerID attribute */
  zb_ieee_addr_t upgrade_server;
  /* FileOffset attribute */
  zb_uint32_t file_offset;
  /* CurrentFileVersion attribute (custom data) */
  zb_uint32_t file_version;
  /* CurrentZigbeeStackVersion attribute */
  zb_uint16_t stack_version;
  /* DownloadedFileVersion attribute */
  zb_uint32_t downloaded_file_ver;
  /* DownloadedZigbeeStackVersion attribute */
  zb_uint16_t downloaded_stack_ver;
  /* ImageUpgradeStatus attribute */
  zb_uint8_t image_status;
  /* Manufacturer ID attribute (custom data) */
  zb_uint16_t manufacturer;
  /* Image Type ID attribute (custom data) */
  zb_uint16_t image_type;
  /* MinimumBlockPeriod attribute */
  zb_uint16_t min_block_period;
  /* Image Stamp attribute */
  zb_uint16_t image_stamp;
} zb_zcl_general_ota_upgrade_attr_t;

static zb_zcl_general_ota_upgrade_attr_t g_general_ota_upgrade_attr = {
  .upgrade_server       = ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE,
  .file_offset          = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE,
  .file_version         = 0x00000000, /* Make sure own version is different from the server to get the file */
  .stack_version        = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO,
  .downloaded_file_ver  = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE,
  .downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE,
  .image_status         = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE,
  .manufacturer         = OTA_UPGRADE_DEFAULT_MANUFACTURER,
  .image_type           = OTA_UPGRADE_DEFAULT_IMAGE_TYPE,
  .min_block_period     = 0,
  .image_stamp          = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE,
};

enum zb_zcl_nxp_ota_cmd_e
{
  ZB_ZCL_CMD_OTA_NXP_STATUS      = 0x00,  /* Status OTA NXP command identifier. */
  ZB_ZCL_CMD_OTA_NXP_GET_FILE    = 0x01,  /* Get file OTA NXP command identifier. */
  ZB_ZCL_CMD_OTA_NXP_ACCEPT_FILE = 0x02,  /* Accept File OTA NXP command identifier. */
#ifdef ZB_STACK_REGRESSION_TESTING_API
  ZB_ZCL_CMD_OTA_NXP_AUTO        = 0x03,  /* Auto OTA NXP command identifier. */
  ZB_ZCL_CMD_OTA_NXP_TEMPO       = 0x04,  /* Tempo OTA NXP command identifier. */
#endif
};
#endif /* CLI_HAS_CLUSTER_OTA_UPGRADE_SRV || CLI_HAS_CLUSTER_OTA_UPGRADE_CLT */


#ifdef CLI_HAS_CLUSTER_THERMOSTAT
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER Thermostat
 *
 * ----------------------------------------------------------------------------------- */

/* Thermostat cluster attributes */
typedef struct {
  zb_int16_t local_temperature;
  zb_int16_t occupied_cooling_setpoint;
  zb_int16_t occupied_heating_setpoint;
  zb_uint8_t control_seq_of_operation;
  zb_uint8_t system_mode;
} zb_zcl_general_thermostat_attr_t;

static zb_zcl_general_thermostat_attr_t g_general_thermostat_attr = {
  .local_temperature = ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_DEFAULT_VALUE,
  .occupied_cooling_setpoint = ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE,
  .occupied_heating_setpoint = ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE,
  .control_seq_of_operation  = ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_DEFAULT_VALUE,
  .system_mode               = ZB_ZCL_THERMOSTAT_CONTROL_SYSTEM_MODE_DEFAULT_VALUE,
};
#endif /* CLI_HAS_CLUSTER_THERMOSTAT */


#ifdef CLI_HAS_CLUSTER_MANUF_SPE
/* -----------------------------------------------------------------------------------
 *
 *                                  CLUSTER NXP CUSTOM
 *
 * ----------------------------------------------------------------------------------- */

typedef zb_uint8_t zb_24bit_t[3];
typedef zb_uint8_t zb_40bit_t[5];
typedef zb_uint8_t zb_48bit_t[6];
typedef zb_uint8_t zb_56bit_t[7];
typedef zb_uint8_t zb_64bit_t[8];
typedef zb_uint8_t zb_128bit_t[16];
typedef struct {
  zb_uint8_t size; /* max size: 255! */
  zb_uint8_t data[256];
} zb_string_t;
typedef struct {
  zb_uint16_t size; /* max size: 255! */
  zb_uint8_t data[256];
} zb_long_string_t;
typedef struct { /* refer to zb_zcl_get_attribute_size() */
  zb_uint8_t       type_null;
  zb_uint8_t       type_8bit;
  zb_uint16_t      type_16bit;
  zb_24bit_t       type_24bit;
  zb_uint32_t      type_32bit;
  zb_40bit_t       type_40bit;
  zb_48bit_t       type_48bit;
  zb_56bit_t       type_56bit;
  zb_64bit_t       type_64bit;
  zb_bool_t        type_bool;
  zb_uint8_t       type_8bitmap;
  zb_uint16_t      type_16bitmap;
  zb_24bit_t       type_24bitmap;
  zb_uint32_t      type_32bitmap;
  zb_40bit_t       type_40bitmap;
  zb_48bit_t       type_48bitmap;
  zb_56bit_t       type_56bitmap;
  zb_64bit_t       type_64bitmap;
  zb_uint8_t       type_u8;
  zb_uint16_t      type_u16;
  zb_24bit_t       type_u24;
  zb_uint32_t      type_u32;
  zb_40bit_t       type_u40;
  zb_48bit_t       type_u48;
  zb_56bit_t       type_u56;
  zb_64bit_t       type_u64;
  zb_int8_t        type_s8;
  zb_int16_t       type_s16;
  zb_24bit_t       type_s24;
  zb_int32_t       type_s32;
  zb_40bit_t       type_s40;
  zb_48bit_t       type_s48;
  zb_56bit_t       type_s56;
  zb_64bit_t       type_s64;
  zb_uint8_t       type_8bit_enum;
  zb_uint16_t      type_16bit_enum;
  zb_uint16_t      type_semi;
  zb_uint32_t      type_single;
  zb_64bit_t       type_double;
  zb_string_t      type_octet_string;
  zb_string_t      type_char_string;
  zb_uint8_t       type_long_octet_string[256];
  zb_long_string_t type_long_char_string;
  zb_uint8_t       type_array[256];
  zb_uint8_t       type_custom_32array[256];
  zb_uint8_t       type_structure[256];
  zb_uint8_t       type_set[256];
  zb_uint8_t       type_bag[256];
  zb_uint32_t      type_time_of_day;
  zb_uint32_t      type_date;
  zb_uint32_t      type_utc_time;
  zb_uint16_t      type_cluster_id;
  zb_uint16_t      type_attribute_id;
  zb_uint32_t      type_bacnet_oid;
  zb_ieee_addr_t   type_ieee_addr;
  zb_128bit_t      type_128_bit_key;
} zb_zcl_custom_nxp_attr_access_t;

typedef struct {
  zb_zcl_custom_nxp_attr_access_t ro; /* read */
  zb_zcl_custom_nxp_attr_access_t ow; /*      write */
  zb_zcl_custom_nxp_attr_access_t rw; /* read write */
} zb_zcl_custom_nxp_attr_t;


static zb_zcl_custom_nxp_attr_t g_custom_nxp_attr = { 0 };

enum zb_zcl_nxp_custom_cmd_e
{
  ZB_ZCL_CMD_NXP_PING    = 0x01,  /* Payload NXP custom command ping identifier. */
  ZB_ZCL_CMD_NXP_ABORT   = 0x02,  /* Payload NXP custom command abort ongoing ping identifier. */
  ZB_ZCL_CMD_NXP_STATUS  = 0x03,  /* Payload NXP custom command status ongoing ping identifier. */
};
#endif /* CLI_HAS_CLUSTER_MANUF_SPE */

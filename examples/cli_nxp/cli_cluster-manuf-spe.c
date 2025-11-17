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
 *                                  CLUSTER CUSTOM NXP
 *
 * ----------------------------------------------------------------------------------- */

#define MAX_PAYLOAD_SIZE 73 /* Maximal payload size authorized in order to send packet in one frame */

static zb_uint8_t custom_nxp_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param);

static zb_uint16_t cluster_revision_nxp_attr_list = ZB_ZCL_BASIC_CLUSTER_REVISION_DEFAULT;

#define CLUSTER_ATTR_FC02_CREATE_ATTR(type, variable, manuf_code) \
  { (type<<8) + 1, type, ACC_READ_ONLY,                                   manuf_code, (void*)&g_custom_nxp_attr.ro.variable  }, \
  { (type<<8) + 2, type,                 ACC_WRITE_ONLY,                  manuf_code, (void*)&g_custom_nxp_attr.ow.variable  }, \
  { (type<<8) + 3, type, ACC_READ_ONLY | ACC_WRITE_ONLY,                  manuf_code, (void*)&g_custom_nxp_attr.rw.variable  }


static zb_zcl_attr_t cluster_attr_fc02[] = {
  /* Mandatory attributes */
  { ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID, ZB_ZCL_ATTR_TYPE_U16, ACC_READ_ONLY,  NO_MANUF_SPE, (void*)&cluster_revision_nxp_attr_list              },
  /*!< Null data type */                               CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_NULL,              type_null,              NO_MANUF_SPE),
  /*!< 8-bit value data type */                        CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_8BIT,              type_8bit,              NO_MANUF_SPE),
  /*!< 16-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_16BIT,             type_16bit,             NO_MANUF_SPE),
  /*!< 24-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_24BIT,             type_24bit,             NO_MANUF_SPE),
  /*!< 32-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_32BIT,             type_32bit,             NO_MANUF_SPE),
  /*!< 40-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_40BIT,             type_40bit,             NO_MANUF_SPE),
  /*!< 48-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_48BIT,             type_48bit,             NO_MANUF_SPE),
  /*!< 56-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_56BIT,             type_56bit,             NO_MANUF_SPE),
  /*!< 64-bit value data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_64BIT,             type_64bit,             NO_MANUF_SPE),
  /*!< Boolean data type */                            CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_BOOL,              type_bool,              NO_MANUF_SPE),
  /*!< 8-bit bitmap data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_8BITMAP,           type_8bitmap,           NO_MANUF_SPE),
  /*!< 16-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_16BITMAP,          type_16bitmap,          NO_MANUF_SPE),
  /*!< 24-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_24BITMAP,          type_24bitmap,          NO_MANUF_SPE),
  /*!< 32-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_32BITMAP,          type_32bitmap,          NO_MANUF_SPE),
  /*!< 40-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_40BITMAP,          type_40bitmap,          NO_MANUF_SPE),
  /*!< 48-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_48BITMAP,          type_48bitmap,          NO_MANUF_SPE),
  /*!< 56-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_56BITMAP,          type_56bitmap,          NO_MANUF_SPE),
  /*!< 64-bit bitmap data type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_64BITMAP,          type_64bitmap,          NO_MANUF_SPE),
  /*!< Unsigned 8-bit value data type */               CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U8,                type_u8,                NO_MANUF_SPE),
  /*!< Unsigned 16-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U16,               type_u16,               NO_MANUF_SPE),
  /*!< Unsigned 24-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U24,               type_u24,               NO_MANUF_SPE),
  /*!< Unsigned 32-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U32,               type_u32,               NO_MANUF_SPE),
  /*!< Unsigned 40-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U40,               type_u40,               NO_MANUF_SPE),
  /*!< Unsigned 48-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U48,               type_u48,               NO_MANUF_SPE),
  /*!< Unsigned 56-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U56,               type_u56,               NO_MANUF_SPE),
  /*!< Unsigned 64-bit value data type */              CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_U64,               type_u64,               NO_MANUF_SPE),
  /*!< Signed 8-bit value data type */                 CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S8,                type_s8,                NO_MANUF_SPE),
  /*!< Signed 16-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S16,               type_s16,               NO_MANUF_SPE),
  /*!< Signed 24-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S24,               type_s24,               NO_MANUF_SPE),
  /*!< Signed 32-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S32,               type_s32,               NO_MANUF_SPE),
  /*!< Signed 40-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S40,               type_s40,               NO_MANUF_SPE),
  /*!< Signed 48-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S48,               type_s48,               NO_MANUF_SPE),
  /*!< Signed 56-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S56,               type_s56,               NO_MANUF_SPE),
  /*!< Signed 64-bit value data type */                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_S64,               type_s64,               NO_MANUF_SPE),
  /*!< 8-bit enumeration (U8 discrete) data type */    CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_8BIT_ENUM,         type_8bit_enum,         NO_MANUF_SPE),
  /*!< 16-bit enumeration (U16 discrete) data type */  CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_16BIT_ENUM,        type_16bit_enum,        NO_MANUF_SPE),
  /*!< 2 byte floating point */                        CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_SEMI,              type_semi,              NO_MANUF_SPE),
  /*!< 4 byte floating point */                        CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_SINGLE,            type_single,            NO_MANUF_SPE),
  /*!< 8 byte floating point */                        CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_DOUBLE,            type_double,            NO_MANUF_SPE),
  /*!< Octet string data type */                       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_OCTET_STRING,      type_octet_string,      NO_MANUF_SPE),
  /*!< Character string (array) data type */           CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_CHAR_STRING,       type_char_string,       NO_MANUF_SPE),
  /*!< Long octet string */                            CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING, type_long_octet_string, NO_MANUF_SPE),
  /*!< Long character string */                        CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_LONG_CHAR_STRING,  type_long_char_string,  NO_MANUF_SPE),
  /*!< Array data type 2 + sum of content len */       CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_ARRAY,             type_array,             NO_MANUF_SPE),
  /*!< Structure data type 2 + sum of content len */   CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_CUSTOM_32ARRAY,    type_custom_32array,    NO_MANUF_SPE),
  /*!< Structure data type 2 + sum of content len */   CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_STRUCTURE,         type_structure,         NO_MANUF_SPE),
  /*!< Collection:set, size = sum of len of content */ CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_SET,               type_set,               NO_MANUF_SPE),
  /*!< Collection:bag, size = sum of len of content */ CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_BAG,               type_bag,               NO_MANUF_SPE),
  /*!< Time of day, 4 bytes */                         CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_TIME_OF_DAY,       type_time_of_day,       NO_MANUF_SPE),
  /*!< Date, 4 bytes */                                CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_DATE,              type_date,              NO_MANUF_SPE),
  /*!< UTC Time, 4 bytes */                            CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_UTC_TIME,          type_utc_time,          NO_MANUF_SPE),
  /*!< Cluster ID, 2 bytes */                          CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_CLUSTER_ID,        type_cluster_id,        NO_MANUF_SPE),
  /*!< Attribute ID, 2 bytes */                        CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_ATTRIBUTE_ID,      type_attribute_id,      NO_MANUF_SPE),
  /*!< BACnet OID, 4 bytes */                          CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_BACNET_OID,        type_bacnet_oid,        NO_MANUF_SPE),
  /*!< IEEE address (U64) type */                      CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_IEEE_ADDR,         type_ieee_addr,         NO_MANUF_SPE),
  /*!< 128-bit security key */                         CLUSTER_ATTR_FC02_CREATE_ATTR(ZB_ZCL_ATTR_TYPE_128_BIT_KEY,       type_128_bit_key,       NO_MANUF_SPE),
  /* End of table */
  { ZB_ZCL_NULL_ID,                         0,                          0,             ZB_MANUFACTURER_CODE_NXP, NULL }
};

void zb_zcl_init_attributes(void)
{
  zb_zcl_custom_nxp_attr_access_t init_val = {
    .type_null            = 0,
    .type_8bit            = 8,
    .type_16bit           = 16,
    .type_24bit           = {1,2,3},
    .type_32bit           = 32,
    .type_40bit           = {1,2,3,4,5},
    .type_48bit           = {1,2,3,4,5,6},
    .type_56bit           = {1,2,3,4,5,6,7},
    .type_64bit           = {1,2,3,4,5,6,7,8},
    .type_bool            = ZB_TRUE,
    .type_8bitmap         = 8,
    .type_16bitmap        = 16,
    .type_24bitmap        = {1,2,3},
    .type_32bitmap        = 32,
    .type_40bitmap        = {1,2,3,4,5},
    .type_48bitmap        = {1,2,3,4,5,6},
    .type_56bitmap        = {1,2,3,4,5,6,7},
    .type_64bitmap        = {1,2,3,4,5,6,7,8},
    .type_u8              = 8,
    .type_u16             = 16,
    .type_u24             = {1,2,3},
    .type_u32             = 32,
    .type_u40             = {1,2,3,4,5},
    .type_u48             = {1,2,3,4,5,6},
    .type_u56             = {1,2,3,4,5,6,7},
    .type_u64             = {1,2,3,4,5,6,7,8},
    .type_s8              = 8,
    .type_s16             = 16,
    .type_s24             = {1,2,3},
    .type_s32             = 32,
    .type_s40             = {1,2,3,4,5},
    .type_s48             = {1,2,3,4,5,6},
    .type_s56             = {1,2,3,4,5,6,7},
    .type_s64             = {1,2,3,4,5,6,7,8},
    .type_8bit_enum       = 8,
    .type_16bit_enum      = 16,
    .type_semi            = 16,
    .type_single          = 32,
    .type_double          = {1,2,3,4,5,6,7,8},
    .type_octet_string      = { 0 },
    .type_char_string       = { 90/*128*/, "the quick brown fox jumps over the lazy dog. THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG. " },
//    .type_char_string       = { 45/*128*/, "the quick brown fox jumps over the lazy dog. " },
    .type_long_octet_string = { 0 },
    .type_long_char_string  = { 0 },
    .type_array             = { 0 },
    .type_custom_32array    = { 0 },
    .type_structure         = { 0 },
    .type_set               = { 0 },
    .type_bag               = { 0 },
    .type_time_of_day       = 0,
    .type_date              = 1747057315,
    .type_utc_time          = 1747057315,
    .type_cluster_id        = 0x3615,
    .type_attribute_id      = 0x3616,
    .type_bacnet_oid        = 0xbac,
    .type_ieee_addr         = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },
    .type_128_bit_key       = { 0xFF },
  };

  ZB_MEMCPY(&g_custom_nxp_attr.ro, &init_val, sizeof(init_val));
  ZB_MEMCPY(&g_custom_nxp_attr.ow, &init_val, sizeof(init_val));
  ZB_MEMCPY(&g_custom_nxp_attr.rw, &init_val, sizeof(init_val));
}

void zb_zcl_test_init_server()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_CUSTOM_NXP,
                              ZB_ZCL_CLUSTER_SERVER_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);

  zb_zcl_init_attributes();
}

void zb_zcl_test_init_client()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_CUSTOM_NXP,
                              ZB_ZCL_CLUSTER_CLIENT_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);

  zb_zcl_init_attributes();
}

static zb_cluster_def cluster_fc02 = {
  cluster_attr_fc02,
  sizeof(cluster_attr_fc02)/sizeof(zb_zcl_attr_t),
  zb_zcl_test_init_server,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  zb_zcl_test_init_client,                          /* Can be replaced by our implementation to configure zb_zcl_cluster_write_attr_hook_t */
  custom_nxp_commands_handler,
};

/* -------------------------------- Cli commands custom NXP --------------------------- */


/* Prototype of custom NXP sub commands */
typedef ZB_PACKED_PRE struct zb_zcl_custom_nxp_ping_hdr_s {
  zb_uint16_t    checksum;
  zb_uint8_t     seq_num;
  void          *identifier; /* Used to store ping_header */
  struct timeval timestamp;
  zb_ret_t       status;
  zb_uint16_t    data_len;
  zb_uint8_t     data_buf[1];
} ZB_PACKED_STRUCT zb_zcl_custom_nxp_ping_hdr_t;

#define ZCL_CUSTNXP_CMD_PING_DEFAULT_CNT    1
#define ZCL_CUSTNXP_CMD_PING_DEFAULT_SIZE   ZCL_CUSTNXP_CMD_PING_MIN_SIZE + 6 /* 7 payload len */
#define ZCL_CUSTNXP_CMD_PING_MIN_SIZE       sizeof(zb_zcl_custom_nxp_ping_hdr_t)
#define ZCL_CUSTNXP_CMD_PING_MIN_FRAG       69 /* ZCL Payload */
#define ZCL_CUSTNXP_CMD_PING_MAX_SIZE       512 /* Could be higher: APS Limits: 8 fragments, payload per fragment 82 */
#define ZCL_CUSTNXP_CMD_PING_DEFAULT_DELAY  1000
static zb_ret_t cluster_custnxp_cmd_ping(int argc, char *argv[]);
static zb_ret_t cluster_custnxp_cmd_abort(int argc, char *argv[]);
static zb_ret_t cluster_custnxp_cmd_status(int argc, char *argv[]);

/* Static command cluster
 * command custom_nxp
 */
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
static zb_ret_t help_custnxp_cmds(void);
#endif
static zb_ret_t help_custnxp_cmds_detailed(char *subcommand);
static zb_ret_t cluster_custnxp_submenu(int argc, char *argv[])
{
  zb_ret_t ret;
  zb_uint8_t command_id;
  cli_tools_strval cmd_table[] = {
    { "ping",    ZB_ZCL_CMD_NXP_PING },
    { "abort",   ZB_ZCL_CMD_NXP_ABORT },
    { "status",  ZB_ZCL_CMD_NXP_STATUS },
  };

  if(argc < 1)
    return RET_INVALID_PARAMETER;

  if(tools_arg_help(argc, argv, 1) == RET_OK)
   return help_custnxp_cmds_detailed(argv[0]);

  /* Search submenu */
  TOOLS_GET_ARG_STRVAL(ret, argv, 0, cmd_table, &command_id);
  if(ret == RET_OK)
  {
    switch(command_id)
    {
      case ZB_ZCL_CMD_NXP_PING:
        return cluster_custnxp_cmd_ping(argc-1, &argv[1]);
      case ZB_ZCL_CMD_NXP_ABORT:
        return cluster_custnxp_cmd_abort(argc-1, &argv[1]);
      case ZB_ZCL_CMD_NXP_STATUS:
        return cluster_custnxp_cmd_status(argc-1, &argv[1]);
    }
  }

  /* ... */

  /* not found, print help */
  menu_printf("cluster custnxp %s: unknown command", argv[0]);
#ifdef MENU_PRINT_HELP_IF_COMMAND_NOT_FOUND
  menu_printf("");
  help_custnxp_cmds();
#endif

  return RET_NO_MATCH;
}
static zb_ret_t help_custnxp_cmds(void) { return help_custnxp_cmds_detailed(NULL); }
static zb_ret_t help_custnxp_cmds_detailed(char *subcommand)
{
  /* SUBMENU CUSTOM NXP Commands */
  if(!subcommand || !strcmp(subcommand, "ping"))
  {
    menu_printf("cluster custom_nxp_cmd ping [endpoint] [dest_addr] [dest_ep] <number> <size> <interval>:");
    menu_printf("\tsend ZCL Command PING from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255], optionaly repeat number <1-65535> payload size <0-%d> delay between cmds in ms <0-65535>", ZCL_CUSTNXP_CMD_PING_MAX_SIZE);
    menu_printf("\tdefault repeat number: %u", ZCL_CUSTNXP_CMD_PING_DEFAULT_CNT);
    menu_printf("\tdefault payload size:  %u, min %u, start fragmenting at %u", ZCL_CUSTNXP_CMD_PING_DEFAULT_SIZE,ZCL_CUSTNXP_CMD_PING_MIN_SIZE, ZCL_CUSTNXP_CMD_PING_MIN_FRAG);
    menu_printf("\tdefault delay:         %u", ZCL_CUSTNXP_CMD_PING_DEFAULT_DELAY);
  }
  if(!subcommand || !strcmp(subcommand, "abort"))
  {
    menu_printf("cluster custom_nxp_cmd abort:");
    menu_printf("\tstop ongoing ping");
  }
  if(!subcommand || !strcmp(subcommand, "status"))
  {
    menu_printf("cluster custom_nxp_cmd status:");
    menu_printf("\tstatus ongoing ping");
  }
  return RET_OK;
}


enum zb_zcl_custom_nxp_cmd_e
{
  ZB_ZCL_CMD_CUSTOM_NXP_PING_ECHO_REQ            = 0x03,
  ZB_ZCL_CMD_CUSTOM_NXP_PING_ECHO_RESP           = 0x04,
};


#undef DEBUG_PING

/* Store ongoing ping */
typedef struct {
  zb_zcl_custom_nxp_ping_hdr_t *header;
  zb_uint8_t                    param;
  struct {
    zb_af_endpoint_desc_t *my_ep;
    zb_uint16_t            dest_short_addr;
    zb_uint8_t             dest_ep_id;
    zb_uint16_t            count;
    zb_uint16_t            size;
    zb_uint16_t            delay;
  } config;
  struct {
    struct timeval start;
    zb_uint16_t    count;   /* nb cmds to send */
    zb_bool_t      ongoing; /* wait tx to be sent */
    zb_uint16_t    tx;      /* nb cmds sent ok */
    zb_uint16_t    rx;      /* nb cmds recv ok */
    struct {
      zb_uint64_t min;
      zb_uint64_t avg;
      zb_uint64_t max;
    } rtt;
  } stats;
} zb_zcl_custom_nxp_ping_info_t;

zb_zcl_custom_nxp_ping_info_t ping = { 0 };

static zb_uint64_t cluster_custnxp_get_elapsed(struct timeval start)
{
  struct timeval now;
  zb_uint64_t elapsed;

  gettimeofday(&now, NULL);

  elapsed = now.tv_sec - start.tv_sec;
  elapsed *= 1000000;
  if(now.tv_usec >= start.tv_usec) { elapsed += (now.tv_usec - start.tv_usec); }
  else {       elapsed -= 1000000;   elapsed += (start.tv_usec - now.tv_usec); }

  return elapsed;
}

static zb_uint16_t cluster_custnxp_checksum(zb_uint8_t *data, zb_uint16_t len)
{
  zb_uint16_t sum = 0;

  for(zb_uint16_t i=0; i<len; i++)
    sum += data[i];

  return sum;
}

static void cluster_custnxp_ping_stats(void)
{
  zb_uint64_t elapsed = cluster_custnxp_get_elapsed(ping.stats.start);

  menu_printf("--- 0x%04x:%u cluster custnxp ping statistics ---", ping.config.dest_short_addr , ping.config.dest_ep_id);
  menu_printf("%u requests, %u acked, %u responses, %u%% requests lost, time %u ms",
    ping.stats.count, ping.stats.tx, ping.stats.rx, ((ping.stats.count - ping.stats.rx) * 100)/ping.stats.count, elapsed/1000);
  menu_printf("rtt min/avg/max = %u.%03u/%u.%03u/%u.%03u ms",
    ping.stats.rtt.min/1000, ping.stats.rtt.min%1000,
    ping.stats.rtt.avg/1000, ping.stats.rtt.avg%1000,
    ping.stats.rtt.max/1000, ping.stats.rtt.max%1000);

  menu_cb_occured();
}

static void cluster_custnxp_ping_send_req_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

#ifdef DEBUG_PING
  WCS_TRACE_DEBUG("%s() id --------, len ---, count %d, max %d, %s", __FUNCTION__,
    ping.stats.count,
    ping.config.count,
    wcs_get_error_str(cmd_send_status->status));
#endif

  if(ping.header)
  {
    /* Still running */
    if(cmd_send_status->status == RET_OK)
    {
      ping.stats.tx++;
    }
  }

  ping.stats.ongoing = ZB_FALSE;

  zb_buf_free(param);
//  menu_cb_occured();
}

static void cluster_custnxp_ping_send_resp_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);

#ifdef DEBUG_PING
  WCS_TRACE_DEBUG("%s() id --------, len ---, count %d, max %d, %s", __FUNCTION__,
    ping.stats.count,
    ping.config.count,
    wcs_get_error_str(cmd_send_status->status));
#endif

  if(cmd_send_status->status == RET_OK)
  {
    /* Do nothing */
  }

  zb_buf_free(param);
//  menu_cb_occured();
}

static zb_ret_t cluster_custnxp_ping_send_cmd(zb_uint16_t                   dest_short,
                                          zb_uint8_t                    dest_ep,
                                          zb_uint8_t                    my_ep,
                                          zb_uint16_t                   profile,
                                          zb_uint16_t                   cluster,
                                          zb_uint8_t                    direction,
                                          zb_uint8_t                    command,
                                          zb_zcl_custom_nxp_ping_hdr_t *ping_header,
                                          zb_callback_t                 cb,
                                          zb_bool_t                     disable_aps_ack)
{
  zb_uint8_t* ptr;
  zb_uint8_t param = zb_buf_get(ZB_FALSE, 2000);

  ptr = zb_buf_begin(param);
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr, direction, ZB_TRUE /* is_manufacturer_specific */, ZB_ZCL_DISABLE_DEFAULT_RESPONSE);
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_TRUE/* is_manufacturer_specific */, ZB_MANUFACTURER_CODE_NXP, command);

  ZB_MEMCPY(ptr, ping_header, sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1);
  ptr += (sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1);

  /* To get the return value, don't use the macro ZB_ZCL_FINISH_N_SEND_PACKET_NEW  since it casts the return value to (void) */
  return zb_zcl_finish_and_send_packet_new(param, ptr, (const zb_addr_u *)(const void *)(&dest_short), ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dest_ep, my_ep, profile, cluster, cb, ZB_TRUE, disable_aps_ack, 0U);
}

static void cluster_custnxp_ping_send_req(zb_uint8_t param)
{
  zb_ret_t ret;
  zb_uint8_t random8;

  ZB_SCHEDULE_APP_ALARM_CANCEL(cluster_custnxp_ping_send_req, ZB_ALARM_ANY_PARAM);

  if(!ping.header)
  {
    /* Aborted */
    return;
  }

  if(ping.stats.ongoing)
  {
    /* One command is already ongoing, try later */
    ZB_SCHEDULE_APP_ALARM(cluster_custnxp_ping_send_req, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(200));
#ifdef DEBUG_PING
    WCS_TRACE_DEBUG("%s(PING_ECHO_REQ) id %08x, count %d, len %d, max %d, POSTPONED", __FUNCTION__,
      ping.header->identifier,
      ping.header->data_len + sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1,
      ping.stats.count,
      ping.config.count);
#endif
    return;
  }

  /* update data */
  ping.header->seq_num++;
  random8 = ZB_RANDOM_U8();
  for(zb_uint16_t i=0; i < ping.header->data_len; i++)
    ping.header->data_buf[i] = (random8+i)%256;
  gettimeofday(&ping.header->timestamp, NULL);
  ping.header->checksum = cluster_custnxp_checksum(&ping.header->seq_num, sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping.header->data_len -1 -2);

  /* send ping req */
  ping.stats.ongoing = ZB_TRUE;
  ret = cluster_custnxp_ping_send_cmd(ping.config.dest_short_addr,
                                     ping.config.dest_ep_id,
                                     ping.config.my_ep->ep_id,
                                     ping.config.my_ep->profile_id,
                                     ZB_ZCL_CLUSTER_ID_CUSTOM_NXP,
                                     ZB_ZCL_FRAME_DIRECTION_TO_SRV,
                                     ZB_ZCL_CMD_CUSTOM_NXP_PING_ECHO_REQ,
                                     ping.header,
                                     cluster_custnxp_ping_send_req_cb,
                                     ZB_FALSE);

#ifdef DEBUG_PING
  WCS_TRACE_DEBUG("%s(PING_ECHO_REQ) id %08x, len %d, count %d, max %d, %s", __FUNCTION__,
    ping.header->identifier,
    ping.header->data_len + sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1,
    ping.stats.count,
    ping.config.count,
    wcs_get_error_str(ret));
#endif

  if(ret != RET_OK)
    menu_printf("ping_send_req() error %s, cannot send ping #%d, %u bytes", wcs_get_error_str(ret), ping.stats.count+1, ping.header->data_len + sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1);
  else
    ping.stats.count++;

  /* prepare next ping req */
  if(ping.stats.count < ping.config.count)
  {
    /* Schedule next ping in ping.config.delay */
    ret = ZB_SCHEDULE_APP_ALARM(cluster_custnxp_ping_send_req, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(ping.config.delay));
    if(ret != RET_OK)
      menu_printf("ping_send_req() error %s, cannot schedule next ping #%d", wcs_get_error_str(ret), ping.stats.count+1);
  }
}

static zb_uint8_t cluster_custnxp_ping_send_resp(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_TRUE; /* Yes, we manage the command */
  zb_ret_t status = RET_OK;
  zb_zcl_custom_nxp_ping_hdr_t *ping_header;
  zb_uint16_t *rx_len; /* In cas the buffer received is too small, use the 2 first bytes of the payload data to store this value & set status to RET_BUFFER_TOO_SMALL */

  if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_ping_hdr_t))
  {
    static zb_zcl_custom_nxp_ping_hdr_t min_header = {0};

    /* Copy whatever we can... */
    ZB_MEMCPY(&min_header, zb_buf_begin(param), zb_buf_len(param));

    ping_header = &min_header;

    rx_len = (zb_uint16_t *)ping_header->data_buf;
    ping_header->status = RET_BUFFER_TOO_SMALL;
    ping_header->data_len = 2;
    *rx_len = zb_buf_len(param);
  }
  else
  {
    ping_header = (zb_zcl_custom_nxp_ping_hdr_t *)zb_buf_begin(param);

    if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1)
    {
      rx_len = (zb_uint16_t *)ping_header->data_buf;
      ping_header->status = RET_BUFFER_TOO_SMALL;
      ping_header->data_len = 2;
      *rx_len = zb_buf_len(param);
    }
    else if(ping_header->checksum != cluster_custnxp_checksum(&ping_header->seq_num, sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1 -2))
      ping_header->status = RET_INVALID_FORMAT;

  }

  ping_header->checksum = cluster_custnxp_checksum(&ping_header->seq_num, sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1 -2);

  /* send ping resp */
  status = cluster_custnxp_ping_send_cmd(ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
                                         ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
                                         ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
                                         cmd_info->profile_id,
                                         cmd_info->cluster_id,
                                         ZB_ZCL_FRAME_DIRECTION_TO_CLI,
                                         ZB_ZCL_CMD_CUSTOM_NXP_PING_ECHO_RESP,
                                         ping_header,
                                         cluster_custnxp_ping_send_resp_cb,
                                         ZB_FALSE);

#ifdef DEBUG_PING
  WCS_TRACE_DEBUG("%s(PING_ECHO_RESP) id %08x, len %d, count %d, max %d, Rx_%s Tx_%s", __FUNCTION__,
    ping_header->identifier,
    ping_header->data_len + sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1,
    ping.stats.count,
    ping.config.count,
    wcs_get_error_str(ping_header->status),
    wcs_get_error_str(status));
#endif

  if(ping_header->status != RET_OK)
    menu_printf("ping_send_resp() recv ping req error %s, %u bytes", wcs_get_error_str(ping_header->status), zb_buf_len(param));
  else if(status != RET_OK)
    menu_printf("ping_send_resp() send ping resp error %s, %u bytes", wcs_get_error_str(status), ping_header->data_len + sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1);

  return ret;
}

static zb_uint8_t cluster_custnxp_ping_recv_resp(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_TRUE; /* Yes, we manage the command */
  zb_ret_t status = RET_OK;
  zb_zcl_custom_nxp_ping_hdr_t *ping_header;
  zb_uint64_t elapsed;
  zb_uint16_t *rx_len; /* In cas the buffer received is too small, use the 2 first bytes of the payload data to store this value & set status to RET_BUFFER_TOO_SMALL */

  ZVUNUSED(cmd_info);

  /* Already aborted */
  if(!ping.header)
    return ret;

  if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_ping_hdr_t))
  {
    zb_zcl_custom_nxp_ping_hdr_t min_header;

    ZB_MEMSET(&min_header, 0xFF, sizeof(zb_zcl_custom_nxp_ping_hdr_t));
    /* Copy whatever we can... */
    ZB_MEMCPY(&min_header, zb_buf_begin(param), zb_buf_len(param));
    status = RET_BUFFER_TOO_SMALL;

    ping_header = &min_header;
  }
  else
  {
    ping_header = (zb_zcl_custom_nxp_ping_hdr_t *)zb_buf_begin(param);
    if(zb_buf_len(param) < sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1)
      status = RET_BUFFER_TOO_SMALL;
    else if(ping_header->checksum != cluster_custnxp_checksum(&ping_header->seq_num, sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1 -2))
      status = RET_INVALID_FORMAT;
  }

  if(status == RET_OK && ping_header->status == RET_OK)
  {
    elapsed = cluster_custnxp_get_elapsed(ping.header->timestamp);
    if(ping.stats.rx == 0)
    {
      ping.stats.rtt.min = elapsed;
      ping.stats.rtt.avg = elapsed;
      ping.stats.rtt.max = elapsed;
    }
    else
    {
      if(elapsed < ping.stats.rtt.min)
        ping.stats.rtt.min = elapsed;
      if(elapsed > ping.stats.rtt.max)
        ping.stats.rtt.max = elapsed;
      ping.stats.rtt.avg = ((ping.stats.rtt.avg * ping.stats.rx) + elapsed)/(ping.stats.rx+1);
    }
    ping.stats.rx++;
  }

  menu_printf("%u bytes from 0x%04x:%u seq_num=%u time=%u.%03u ms, status tx_%s/rx_%s",
    sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping_header->data_len -1,
     ping.config.dest_short_addr,
     ping.config.dest_ep_id,
     ping_header->seq_num,
     elapsed/1000, elapsed%1000,
     wcs_get_error_str(ping_header->status),
     wcs_get_error_str(status));
  switch(ping_header->status) {
    case RET_BUFFER_TOO_SMALL:
      rx_len = (zb_uint16_t *)ping_header->data_buf;

      menu_printf("\tserver has received only %u of %u bytes", *rx_len, ping.config.size + sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1);
      break;
    case RET_INVALID_FORMAT:
      menu_printf("\tserver has detected a bad checksum");
      break;
  }

  if(ping.stats.count == ping.config.count)
  {
    ZB_SCHEDULE_APP_ALARM_CANCEL(cluster_custnxp_ping_send_req, ZB_ALARM_ANY_PARAM);
    cluster_custnxp_ping_stats();
    ZB_FREE(ping.header);
    ping.header = NULL;
    zb_buf_free(ping.param);
    ping.param = 0;
    ping.stats.ongoing = ZB_FALSE;
  }

  return ret;
}

/* Static command cluster
 * command custom_nxp_cmd
 *
 * cluster custom_nxp_cmd ping [endpoint] [dest_addr] [dest_ep] <number> <size> <interval>
 */
static zb_ret_t cluster_custnxp_cmd_ping(int argc, char *argv[])
{
  zb_ret_t ret;

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc < 3 || argc > 6)
    return RET_INVALID_PARAMETER;

  if(ping.header)
    return RET_BUSY;

  ping.config.my_ep = NULL;

  /*  get [endpoint] & validate it has cluster custom_nxp client */
  TOOLS_GET_ARG_ENDPOINT_W_CL(ret, argv, 0, &ping.config.my_ep, ZB_ZCL_CLUSTER_ID_CUSTOM_NXP, ZB_ZCL_CLUSTER_CLIENT_ROLE);

  /* get [dest_addr] */
  TOOLS_GET_ARG_HEXA(ret, uint16, argv, 1, &ping.config.dest_short_addr);

  /* get [dest_ep] */
  TOOLS_GET_ARG(ret, uint8,  argv, 2, &ping.config.dest_ep_id);

  if(argc >= 4)
  {
    /* get <number> */
    TOOLS_GET_ARG(ret, uint16,  argv, 3, &ping.config.count);
  }
  else
    ping.config.count = ZCL_CUSTNXP_CMD_PING_DEFAULT_CNT;

  if(argc >= 5)
  {
    /* get <size> */
    TOOLS_GET_ARG(ret, uint16,  argv, 4, &ping.config.size);
    if(ping.config.size < sizeof(zb_zcl_custom_nxp_ping_hdr_t) || ping.config.size > ZCL_CUSTNXP_CMD_PING_MAX_SIZE)
      return RET_INVALID_PARAMETER_4;
  }
  else
    ping.config.size = ZCL_CUSTNXP_CMD_PING_DEFAULT_SIZE;

  /* Remove header size */
  ping.config.size -= (sizeof(zb_zcl_custom_nxp_ping_hdr_t) - 1);

  if(argc >= 6)
  {
    /* get <interval> */
    TOOLS_GET_ARG(ret, uint16,  argv, 5, &ping.config.delay);
  }
  else
    ping.config.delay = ZCL_CUSTNXP_CMD_PING_DEFAULT_DELAY;

  /* Do it */
  {
    ZB_MEMSET(&ping.stats, 0, sizeof(ping.stats));
    gettimeofday(&ping.stats.start, NULL);

    ping.param = zb_buf_get_any();
    ping.header = (zb_zcl_custom_nxp_ping_hdr_t *)ZB_MALLOC(sizeof(zb_zcl_custom_nxp_ping_hdr_t) + ping.config.size - 1/* header already contain 1 byte of data */);
    if(!ping.header)
      return RET_NO_MEMORY;

    ping.header->seq_num    = (zb_uint8_t)-1;
    ping.header->identifier = (void *)ping.header;
    /* ping.header->timestamp  = Updated by each ping */
    ping.header->status     = RET_OK;
    ping.header->data_len   = ping.config.size;

    ZB_SCHEDULE_APP_CALLBACK(cluster_custnxp_ping_send_req, ping.param);
  }

  return RET_OK;
}


/* Static command cluster
 * command custom_nxp_cmd
 *
 * cluster custom_nxp_cmd abort
 */
static zb_ret_t cluster_custnxp_cmd_abort(int argc, char *argv[])
{

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 0)
    return RET_INVALID_PARAMETER;
  ZVUNUSED(argv);

  if(ping.header)
  {
    ZB_SCHEDULE_APP_ALARM_CANCEL(cluster_custnxp_ping_send_req, ZB_ALARM_ANY_PARAM);
    cluster_custnxp_ping_stats();
    ZB_FREE(ping.header);
    ping.header = NULL;
    zb_buf_free(ping.param);
    ping.param = 0;
    ping.stats.ongoing = ZB_FALSE;
  }

  return RET_OK;
}


/* Static command cluster
 * command custom_nxp_cmd
 *
 * cluster custom_nxp_cmd status
 */
static zb_ret_t cluster_custnxp_cmd_status(int argc, char *argv[])
{

  if(!config_is_started())
    return RET_UNAUTHORIZED;

  if(argc != 0)
    return RET_INVALID_PARAMETER;
  ZVUNUSED(argv);

  menu_printf("% ping status: total %d, sent %d/%d, recv %d",
    (ping.header)?("ongoing"):("last"),
    ping.config.count,
    ping.stats.count,
    ping.stats.tx,
    ping.stats.rx);

  return RET_OK;
}


/* -------------------------------- Handler commands custom NXP --------------------------- */

static zb_uint8_t custom_nxp_server_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  /* This is just for information, let the stack manage it */
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_CUSTOM_NXP_PING_ECHO_REQ:
      ret = cluster_custnxp_ping_send_resp(cmd_info, param);
      break;

    default:
      menu_printf("%s() recv from 0x%04x endpoint %u to %u:%s: unknown cmd 0x%04x, call dummy handler",
        __FUNCTION__,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
        (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
        cmd_info->cmd_id);
        dummy_commands_handler(cmd_info, param);
      break;

  }

 // We handled the packet, free it
  if(ret == ZB_TRUE)
    zb_buf_free(param);

  return ret;
}

static zb_uint8_t custom_nxp_client_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  zb_uint8_t ret = ZB_FALSE;

  switch(cmd_info->cmd_id) {
    case ZB_ZCL_CMD_CUSTOM_NXP_PING_ECHO_RESP:
      ret = cluster_custnxp_ping_recv_resp(cmd_info, param);
      break;

    default:
      menu_printf("%s() recv from 0x%04x endpoint %u to %u:%s: unknown cmd 0x%04x, call dummy handler",
        __FUNCTION__,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint,
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint,
        (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?("CLIENT"):("SERVER"),
        cmd_info->cmd_id);
        dummy_commands_handler(cmd_info, param);
      break;
  }

 // We handled the packet, free it
  if(ret == ZB_TRUE)
    zb_buf_free(param);

  return ret;
}

static zb_uint8_t custom_nxp_commands_handler(zb_zcl_parsed_hdr_t *cmd_info, zb_uint8_t param)
{
  return (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_CLI)?(custom_nxp_client_commands_handler(cmd_info, param)):(custom_nxp_server_commands_handler(cmd_info, param));
}

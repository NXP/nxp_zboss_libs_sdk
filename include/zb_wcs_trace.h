/*
 *
 * Copyright 2023-2025 NXP
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
 *
 */

#ifndef ZB_WCS_LOGGER_H
#define ZB_WCS_LOGGER_H 1

#ifndef ZB_MACSPLIT_DEVICE_NXP

#include "zb_config.h"
#include "zb_types.h"
#include "zb_errors.h"

zb_uint8_t  wcs_config_trace_level(zb_uint8_t default_level);
zb_uint32_t wcs_config_trace_mask(zb_uint32_t default_mask);

typedef enum {
  WCS_LOG_LEVEL_ERROR=0,
  WCS_LOG_LEVEL_WARNING,
  WCS_LOG_LEVEL_NOTICE,
  WCS_LOG_LEVEL_INFO,
  WCS_LOG_LEVEL_DBGREL, /* DEBUG traces visible is release build */
#ifdef DEBUG
  WCS_LOG_LEVEL_DEBUG,  /* DEBUG traces NOT visible is release build */
#endif
} wcs_log_level_t;

extern int wcs_out;

#define WCS_TRACE_ERROR(format, ...)   do { if(wcs_out) wcs_printf(WCS_LOG_LEVEL_ERROR,   format, ##__VA_ARGS__); } while(0)
#define WCS_TRACE_WARNING(format, ...) do { if(wcs_out) wcs_printf(WCS_LOG_LEVEL_WARNING, format, ##__VA_ARGS__); } while(0)
#define WCS_TRACE_NOTICE(format, ...)  do { if(wcs_out) wcs_printf(WCS_LOG_LEVEL_NOTICE,  format, ##__VA_ARGS__); } while(0)
#define WCS_TRACE_INFO(format, ...)    do { if(wcs_out) wcs_printf(WCS_LOG_LEVEL_INFO,    format, ##__VA_ARGS__); } while(0)
#define WCS_TRACE_DBGREL(format, ...)  do { if(wcs_out) wcs_printf(WCS_LOG_LEVEL_DBGREL,  format, ##__VA_ARGS__); } while(0) /* DEBUG traces visible is release build */
#ifdef DEBUG
#define WCS_TRACE_DEBUG(format, ...)  do { if(wcs_out)  wcs_printf(WCS_LOG_LEVEL_DEBUG,   format, ##__VA_ARGS__); } while(0) /* DEBUG traces NOT visible is release build */
#else
#define WCS_TRACE_DEBUG(format, ...)   do {} while(0)
#endif


void wcs_trace_config(const char *name, char *env);

int wcs_snprintf(char *str, size_t size, const char *format, ...);
int wcs_vsnprintf(char *str, size_t size, const char *format, va_list ap);
void wcs_flush_trace(void);

#ifdef ZB_USE_LOGFILE_ROTATE
void wcs_check_and_rotate_trace_file(zb_bool_t force_rotate);
#endif

#ifdef ZB_LOGFILE_POST_ROTATE_SH
void wcs_trace_post_rotate(const char *path, char *filename);
#endif

int wcs_printf(wcs_log_level_t level, const char *format, ...);

void wcs_print_buf(const zb_uint8_t *buffer, zb_int_t len, const char *format, ...);

void wcs_print_signal(zb_uint8_t sig, zb_uint8_t status, void *sg_p);
void wcs_print_error(zb_uint8_t severity, zb_ret_t error_code, void *additional_info);

void wcs_print_zdo_data(zb_bool_t tx, char *msg, zb_uint16_t clusterid);

#ifdef ZB_MACSPLIT_HOST
void wcs_dump_macsplit_msg(zb_bool_t tx, zb_uint16_t call_type, zb_uint8_t bufid);
void wcs_fill_macsplit_buf(char *line_buf, int line_size, zb_bool_t tx, void *packet);
#endif

char *get_cmd_id_str(zb_bool_t common_command, zb_uint16_t cluster_id, zb_uint8_t cmd_id);
char *get_endpoint_profile_id_str(zb_uint16_t profile_id);
char *get_endpoint_device_id_str(zb_uint16_t device_id);
char *get_nvram_dataset_str(zb_uint16_t dataset_id);
char *get_cluster_id_str(zb_uint16_t cluster_id);
char *get_cluster_attr_id_str(zb_uint16_t cluster_id, zb_uint16_t attr_id);
char *get_attr_type_str(zb_uint8_t type);
char *get_attr_access_str(zb_uint8_t access);
char *get_device_cb_id_str(zb_uint8_t device_cb);
char *get_err_sev_str(zb_uint8_t severity);
char *wcs_get_error_str(zb_ret_t retval);
char *get_zcl_status_str(zb_uint8_t zcl_status);
char *get_zdp_status_str(zb_uint8_t zdp_status);

#endif /* ZB_MACSPLIT_DEVICE_NXP */
#endif /* ZB_WCS_LOGGER_H */

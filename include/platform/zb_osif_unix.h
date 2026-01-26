/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
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
/* PURPOSE: OS and platform depenednt stuff for Unix platform
*/

#ifndef ZB_OSIF_UNIX_H
#define ZB_OSIF_UNIX_H 1

/*! \addtogroup ZB_OSIF_UNIX */
/*! @{ */

#include "zb_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <netinet/in.h>
#ifndef __MINGW32__
#include <poll.h>
#endif
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#define ZB_ABORT abort

int osif_is_term_sig_received(void);
#define ZB_OSIF_IS_EXIT() osif_is_term_sig_received()

#ifndef UNIX_DOMAIN_SOCKETS_DIR
#define UNIX_DOMAIN_SOCKETS_DIR "/tmp/"
#endif

#define ZB_SDCC_XDATA
#define ZB_CALLBACK
#define ZB_SDCC_BANKED
#define ZB_KEIL_REENTRANT

/* Unix-specific trace */

#ifndef ZB_FILE_CONFIG_DELIMITERS
#define ZB_FILE_CONFIG_DELIMITERS "=\n"
#endif

typedef enum zb_file_config_item_type_e
{
  ZB_FILE_CONFIG_ITEM_STRING_TYPE,
  ZB_FILE_CONFIG_ITEM_NUMBER_TYPE
} zb_file_config_item_type_t;

typedef struct zb_file_config_item_s
{
  const char *name;
  void       *value;
  int         value_size;
  int         type;  /* zb_file_config_item_type_t */
} zb_file_config_item_t;

#define ZB_FILE_CONFIG_EXISTS(config) ((config)[0] != '\0')
zb_ret_t zb_file_config_read(const char * config_file_name, int items_count, zb_file_config_item_t *items);
zb_ret_t zb_file_config_update(const char * config_file_name, int items_count, zb_file_config_item_t *items);

#define ZB_PATH_MAX PATH_MAX

#ifdef ZB_FILE_PATH_MGMNT
#ifdef ZB_MNTFS_FOR_TRACE_LOGS
#define ZB_FILE_PATH_BASE_TRACE_LOGS ZB_FILE_PATH_BASE_MNTFS_TRACE_LOGS
#else
#define ZB_FILE_PATH_BASE_TRACE_LOGS ZB_FILE_PATH_BASE_RAMFS_TRACE_LOGS
#endif /* ZB_MNTFS_FOR_TRACE_LOGS */

/* paths config files */
#ifndef RFS_CONFIG_FILE_NAME
#define RFS_CONFIG_FILE_NAME "rfs_file_path_config"
#endif
#ifndef MNTFS_CONFIG_FILE_NAME
#define MNTFS_CONFIG_FILE_NAME "mntfs_file_path_config"
#endif
#ifndef RAMFS_CONFIG_FILE_NAME
#define RAMFS_CONFIG_FILE_NAME "ramfs_file_path_config"
#endif

#ifndef ZB_BASE_FILE_PATH_PREFIX
#define ZB_BASE_FILE_PATH_PREFIX ""
#endif

#ifndef RFS_CONFIG_FILE_PATH
#define RFS_CONFIG_FILE_PATH   ZB_BASE_FILE_PATH_PREFIX RFS_CONFIG_FILE_NAME
#endif
#ifndef MNTFS_CONFIG_FILE_PATH
#define MNTFS_CONFIG_FILE_PATH ZB_BASE_FILE_PATH_PREFIX MNTFS_CONFIG_FILE_NAME
#endif
#ifndef RAMFS_CONFIG_FILE_PATH
#define RAMFS_CONFIG_FILE_PATH ZB_BASE_FILE_PATH_PREFIX RAMFS_CONFIG_FILE_NAME
#endif

#ifndef RFS_BINARIES_DIR
#define RFS_BINARIES_DIR "rfs_binaries_dir"
#endif
#ifndef MNTFS_BINARIES_DIR
#define MNTFS_BINARIES_DIR "mntfs_binaries_dir"
#endif
#ifndef MNTFS_USERDATA_DIR
#define MNTFS_USERDATA_DIR "mntfs_userdata_dir"
#endif
#ifdef ZB_MNTFS_FOR_TRACE_LOGS
#ifndef MNTFS_LOGS_DIR
#define MNTFS_LOGS_DIR "mntfs_logs_dir"
#endif
#endif
#ifndef RAMFS_LOGS_DIR
#define RAMFS_LOGS_DIR "ramfs_logs_dir"
#endif
#ifndef RAMFS_UNIX_DOMAIN_SOCK_DIR
#define RAMFS_UNIX_DOMAIN_SOCK_DIR "ramfs_unix_domain_sock_dir"
#endif
#ifndef RAMFS_TMP_DIR
#define RAMFS_TMP_DIR "ramfs_tmp_dir"
#endif

void zb_file_path_init_from_config(const char * process_path, const char * base_config_file_name,
                                   const char * config_file_name, int items_count, zb_file_config_item_t *items);
#endif /* ZB_FILE_PATH_MGMNT */

typedef FILE zb_osif_file_t;
typedef zb_uint32_t zb_minimal_vararg_t;

#ifdef ZB_THREADS
void zb_osif_global_lock();
void zb_osif_global_unlock();
#define ZB_OSIF_GLOBAL_LOCK()   TRACE_MSG(TRACE_MACLL4, "lock", (FMT__0)); zb_osif_global_lock()
#define ZB_OSIF_GLOBAL_UNLOCK() TRACE_MSG(TRACE_MACLL4, "unlock", (FMT__0)); zb_osif_global_unlock()
void zb_osif_time_lock(void);
void zb_osif_time_unlock(void);
#define ZB_OSIF_TIME_LOCK()   zb_osif_time_lock()
#define ZB_OSIF_TIME_UNLOCK() zb_osif_time_unlock()
#else
#define ZB_OSIF_GLOBAL_LOCK() (void)0
#define ZB_OSIF_GLOBAL_UNLOCK() (void)0
#endif

#define ZB_XDATA
#define ZB_CODE
#define ZB_STOP_WATCHDOG()

#define DECLARE_SERIAL_INTER_HANDLER()

/* use macros to be able to redefine */
#define ZB_MALLOC   malloc
#define ZB_CALLOC   calloc
#define ZB_REALLOC  realloc
#define ZB_FREE     free

#define ZB_DO_EXIT()  exit(0)

#define ZB_MEMCPY memcpy
#define ZB_MEMMOVE memmove
#define ZB_MEMSET memset
#define ZB_MEMCMP memcmp
#define ZB_MEMCHR memchr

#define ZB_STRERROR() strerror(errno)

#define ZB_PRINT_OUT(format, ...)  fprintf(stdout, format, ##__VA_ARGS__)
#define ZB_PRINT_ERR(format, ...)  fprintf(stderr, format, ##__VA_ARGS__)

#define ZB_TOUPPER toupper
#define ZB_TOLOWER tolower

#define ZVUNUSED(v) (void)v

#ifdef ZB_STACK_REGRESSION_TESTING_API
#define ZB_ENABLE_ALL_INTER()  ZB_REGRESSION_TESTS_API().osif_interrupts_balance++
#define ZB_DISABLE_ALL_INTER() ZB_REGRESSION_TESTS_API().osif_interrupts_balance--
#else
#define ZB_ENABLE_ALL_INTER()
#define ZB_DISABLE_ALL_INTER()
#endif /* ZB_STACK_REGRESSION_TESTING_API */


#define ZB_VOLATILE

#define ZB_BZERO(s,l) ZB_MEMSET((char*)(s), 0, (l))
#define ZB_BZERO2(s) ZB_BZERO(s, 2)

void zb_unix_platform_init(void);
void zb_osif_init_timer(void);

#define ZB_PLATFORM_INIT() zb_unix_platform_init()

#define ZB_TIMER_INIT() zb_osif_init_timer()
#define ZB_CHECK_TIMER_IS_ON() 1 /*!< always on in linux */
#define ZB_START_HW_TIMER() /*!< nothing to do here */

/* Network routines */

#define OSIF_DECLARE_IPC_WAIT_CONTROL_T(type_name, n) typedef struct pollfd type_name[n]

typedef int osif_ipc_handle_t;
typedef void* osif_wait_control_t;
typedef pid_t osif_pid_t;
typedef int osif_control_pipe_t[2];

#define OSIF_IPC_SIGNAL_RX  1
#define OSIF_IPC_SIGNAL_TX  2
#define OSIF_IPC_SIGNAL_ERR 4
#define OSIF_IPC_SIGNAL_HUP 8
#define OSIF_IPC_SIGNAL_EXCEPT 16

#define OSIF_IPC_INVALID_H -1

typedef void (*osif_io_handler_t)(osif_ipc_handle_t handle, zb_uint8_t event_mask);

void osif_ipc_init(void);
void osif_ipc_deinit(void);
zb_ret_t osif_ipc_add_io_handler(osif_ipc_handle_t handle,
                                       zb_uint8_t event_mask,
                                       osif_io_handler_t handler);
zb_ret_t osif_ipc_remove_io_handler(osif_ipc_handle_t handle);
void zb_scheduler_wakeup(void);

void zb_osif_platform_io_iteration(zb_bool_t block);

#if defined ZB_COMPILE_MAC_MONOLITHIC
#define ZB_TRANSPORT_BLOCK() zb_nsng_io_iteration(ZB_TRUE)
#define ZB_TRANSPORT_NONBLOCK_ITERATION() zb_nsng_io_iteration(ZB_FALSE)
#else
#define ZB_TRANSPORT_BLOCK() zb_osif_platform_io_iteration(ZB_TRUE)
#define ZB_TRANSPORT_NONBLOCK_ITERATION() zb_osif_platform_io_iteration(ZB_FALSE)
#endif /* ZB_COMPILE_MAC_MONOLITHIC */


void zb_unix_platform_init(void);

void osif_net_init();
char *osif_getenv(char *key, char *def);
void osif_gettime_us(zb_uint64_t *time_us);
void osif_get_clock_monotonic_raw(struct timespec *t);
zb_uint64_t osif_get_clock_delta_ns(struct timespec *t_old, struct timespec *t_new);
zb_uint64_t osif_get_clock_delta_us(struct timespec *t_old, struct timespec *t_new);
zb_uint64_t osif_get_clock_delta_ms(struct timespec *t_old, struct timespec *t_new);
void osif_get_clock_realtime(struct timespec *t);
zb_uint32_t osif_timespec_to_seconds(struct timespec *t);
void osif_seconds_to_timespec(zb_uint32_t s, struct timespec *t);
zb_uint32_t osif_get_clock_monotonic_sec();
zb_uint32_t osif_get_clock_realtime_sec();
int osif_timespec_compare(struct timespec *t1, struct timespec *t2);
zb_uint32_t osif_timespec_diff_us_get(struct timespec *t1, struct timespec *t2);
void osif_timespec_add_sec(struct timespec *t, zb_uint32_t sec);
void osif_timespec_add_ms(struct timespec *t, zb_uint32_t ms);
void osif_timespec_add_us(struct timespec *t, zb_uint32_t us);
void osif_timespec_add_ns(struct timespec *t, zb_uint32_t ns);
zb_uint32_t osif_current_time_to_be();
zb_uint64_t osif_timespec_to_ms(struct timespec *t);
void osif_mseconds_to_timespec(zb_uint64_t ms, struct timespec *t);
zb_uint64_t osif_get_clock_monotonic_ms();
zb_uint64_t osif_timespec_to_us(struct timespec *t);
void osif_useconds_to_timespec(zb_uint64_t us, struct timespec *t);
zb_uint64_t osif_get_clock_monotonic_us();
zb_uint64_t osif_timespec_to_ns(struct timespec *t);
void osif_nseconds_to_timespec(zb_uint64_t ns, struct timespec *t);
zb_uint64_t osif_get_clock_monotonic_ns();
unsigned osif_getuid();
zb_int32_t osif_gethostid();
zb_int_t osif_ipc_create_h(osif_ipc_handle_t *h_p);
zb_int_t osif_tcp_create_h(osif_ipc_handle_t *h_p);
zb_int_t osif_ipc_srv_init(char *key, osif_ipc_handle_t *h_p);
zb_int_t osif_ipc_accept(osif_ipc_handle_t srv_h, osif_ipc_handle_t *h_p);
zb_int_t osif_ipc_connect(char *key, osif_ipc_handle_t h);
zb_int_t osif_tcp_connect(char *host, unsigned port, osif_ipc_handle_t h);
zb_int_t osif_tcp_connect_w_timeout(char *host, unsigned port, osif_ipc_handle_t h, zb_uint8_t timeout_sec);
zb_int_t osif_ipc_write(osif_ipc_handle_t h, const void *buf, zb_int_t len);
zb_int_t osif_ipc_read(osif_ipc_handle_t h, void *buf, zb_int_t len);
void osif_ipc_close(osif_ipc_handle_t h);
zb_int_t osif_ipc_wait_for_io(osif_wait_control_t wait_control, zb_int_t n_handlers, zb_uint_t timeout_us);
zb_int_t osif_ipc_poll_io(osif_wait_control_t wait_control, zb_int_t n_handlers, zb_uint_t timeout_us);
zb_int_t osif_ipc_select_io(osif_wait_control_t wait_control, zb_int_t n_handlers, zb_uint_t timeout_us);
osif_ipc_handle_t osif_ipc_get_handler(osif_wait_control_t wait_control, zb_int_t i);
zb_int_t osif_ipc_signaled(osif_wait_control_t wait_control, zb_int_t i, zb_uint_t *mask_p);
void osif_ipc_init_wait(osif_wait_control_t wait_control, zb_int_t nfds);
void osif_ipc_setup_wait(osif_wait_control_t wait_control, zb_int_t i, osif_ipc_handle_t h, zb_uint8_t actions);
void osif_ipc_remove_wait(osif_wait_control_t wait_control, zb_int_t i);

zb_ret_t osif_ctrl_pipe_init(osif_control_pipe_t *ctrl);
zb_ret_t osif_ctrl_pipe_deinit(osif_control_pipe_t ctrl);
zb_ret_t osif_ctrl_pipe_event_set(osif_control_pipe_t ctrl, zb_char_t e);
zb_ret_t osif_ctrl_pipe_exit_event_set(osif_control_pipe_t ctrl);
zb_ret_t osif_ctrl_pipe_event_read(osif_control_pipe_t ctrl, zb_char_t *e);
void osif_ctrl_pipe_event_flush(osif_control_pipe_t ctrl);
void osif_ctrl_pipe_event_flush_all(osif_control_pipe_t ctrl);
osif_ipc_handle_t osif_ctrl_pipe_get_handler(osif_control_pipe_t ctrl);

void osif_sleep(zb_uint_t s);
void osif_usleep(zb_uint_t us);
void zb_osif_wait_ms(zb_uint16_t ms);
osif_pid_t osif_getpid();
void osif_srand(unsigned seed);
int osif_rand();
void osif_terminate_process(osif_pid_t pid);

/* Get  HW long address */
void zb_osif_get_ieee_eui64(zb_ieee_addr_t ieee_eui64);

/* 03/10/2015 CR [MZ] Block start. */
zb_ret_t osif_net_get_local_ip_address(osif_ipc_handle_t *, zb_char_t *);
/* 03/10/2015 CR [MZ] Block end. */
#define MAC_ADDRESS_SIZE 6
/* MAC addr string: 6 hex bytes (12 chars) + str termination ('\0') */
#define MAC_ADDRESS_STRING_SIZE (MAC_ADDRESS_SIZE * 2 + 1)
/* MAC addr string with colons: 6 hex bytes (12 chars) + 5 colons + str termination ('\0') */
#define MAC_ADDRESS_WITH_COLONS_STRING_SIZE (MAC_ADDRESS_SIZE * 2 + 5 + 1)
zb_ret_t osif_net_get_mac_address(unsigned char * mac_address, char *ifname);
zb_ret_t osif_net_get_ip4_address(struct in_addr *ip4_address, char *ifname);
zb_ret_t osif_net_get_mac_address_sys(unsigned char * mac_address, char *ifname);

/* threads */
typedef pthread_cond_t     osif_cond_t;
typedef pthread_condattr_t osif_condattr_t;
typedef pthread_mutex_t    osif_mutex_t;
typedef pthread_t          osif_thread_t;
typedef void *osif_func_ret_t;
typedef void *osif_func_arg_t;
typedef osif_func_ret_t (*osif_func_t)(osif_func_arg_t);
typedef osif_func_ret_t (*osif_tread_detached_func_t)(osif_func_arg_t);

#define OSIF_THREAD_STACK_DEFINE(stack_name, stack_size)

zb_ret_t   osif_start_thread(osif_thread_t *thread, osif_func_t func, void * arg);
#define    osif_start_thread2(thread, func, arg, stack_name, stack_size)            osif_start_thread(thread, func, arg)
zb_ret_t   osif_start_thread_detached(osif_func_t func, void * arg);
void* osif_thread_join(osif_thread_t *thread);
zb_uint_t  osif_get_thread_id(ZB_VOID_ARGLIST);
zb_ret_t   osif_detach_thread(osif_thread_t *handle);
void       osif_thread_exit(void *retval);

zb_ret_t osif_init_mutex(osif_mutex_t *mutex);
zb_ret_t osif_deinit_mutex(osif_mutex_t *mutex);
zb_ret_t osif_lock_mutex(osif_mutex_t *mutex);
zb_ret_t osif_trylock_mutex(osif_mutex_t *mutex);
zb_ret_t osif_unlock_mutex(osif_mutex_t *mutex);


zb_ret_t osif_init_cond(osif_cond_t *cond);
zb_ret_t osif_init_cond_with_attr(osif_cond_t *cond, osif_condattr_t *condattr);
zb_ret_t osif_deinit_cond(osif_cond_t *cond);
zb_ret_t osif_init_condattr(osif_condattr_t *condattr);
zb_ret_t osif_deinit_condattr(osif_condattr_t *condattr);
zb_ret_t osif_condattr_setclock(osif_condattr_t *condattr, zb_uint8_t clockid);
zb_ret_t osif_cond_wait(osif_cond_t *cond, osif_mutex_t *mutex);
zb_ret_t osif_cond_wait_with_timeout(osif_cond_t *cond, osif_mutex_t *mutex, unsigned wait_interval_ms);
zb_ret_t osif_cond_wait_with_target(osif_cond_t *cond, osif_mutex_t *mutex, struct timespec *target);
zb_ret_t osif_cond_signal(osif_cond_t *cond);
zb_ret_t osif_cond_broadcast(osif_cond_t *cond);
void osif_sched_yield();

void zb_osif_update_timer(void);

#define ZB_CHECK_TIMER_IS_ON() 1 /*!< always on in linux */
#define ZB_START_HW_TIMER() /*!< nothing to do here */
#define ZB_STOP_HW_TIMER() /*!< nothing to do here */

zb_ret_t osif_net_get_ifname_for_socket(osif_ipc_handle_t h, zb_char_t *buf, zb_uint16_t bufsize);

int zb_file_get_current_process_path(zb_char_t *buf, zb_uint16_t len);
int zb_file_get_current_process_dir(zb_char_t *buf, zb_uint16_t len);

zb_bool_t zb_osif_check_dir_exist(const zb_char_t *name);
int zb_osif_create_dir(const zb_char_t *name);

typedef void (*pipe_notification_cb_t)(zb_char_t e);

#ifdef ZB_MAIN_THREAD_PIPE_NOTIFICATION
void zb_register_pipe_notification_handler(pipe_notification_cb_t cb);
#endif  /* ZB_MAIN_THREAD_PIPE_NOTIFICATION */

typedef struct zb_osif_daemon_wait_s
{
  int pipe[2];
} zb_osif_daemon_wait_t;

/**
  Indicate the parent process the status of
  its child initialization that parent can wait

   @param w       - the structure of wait pipe context between two processes
                    Please note, pipe deinitialization is performed by this function

   @param ret     - the initialization status of the child process

   @return nothing

 */
void zb_osif_daemonize_complete(zb_osif_daemon_wait_t *w, zb_ret_t ret);

/**
  Daemonize the caller
  The caller can wait the completion of child initialization

   @param w       - the structure of wait pipe context between two processes.
                    Could be NULL, in this case Parent is not going to wait

                    Please note, pipe initialization/deinitialization is performed by this function

   @return RET_OK in case of success, otherwise the error

           Please note, this return code will be used for both Parent and Child.
           The Child, in case of Success initialization, can pass RET_EXIT to
           indicate the Parent the initialization is complete and the Parent
           can exit

 */
zb_ret_t zb_osif_daemonize(zb_osif_daemon_wait_t *w);

/* READLINE */

/**
  Initialize read line from stdin

   @return RET_OK in case of success, otherwise the error
 */
zb_ret_t zb_osif_readline_init(void);

/**
  Read line from stdin

   @param prompt  - Prompt to be displayed before reading a line

   @return line string
 */
char *zb_osif_readline_get(const char *prompt);

/**
  Free line from stdin

   @param line  - Prompt to be displayed before reading a line

   @return line string
 */
void zb_osif_readline_free(char *line);

/**
  Add line to history

   @param line  - Line to add

   @return RET_OK in case of success, otherwise the error
 */
zb_ret_t zb_osif_readline_add_history(const char *line);

/* GPIO */

#define ZB_GPIO_MAX_FD_PATH_LEN 128

typedef enum zb_gpio_direction_e
{
  ZB_GPIO_DIR_IN = 0,
  ZB_GPIO_DIR_OUT
} zb_gpio_direction_t;

typedef enum zb_gpio_edge_e
{
  ZB_GPIO_EDGE_NONE = 0,
  ZB_GPIO_EDGE_RISING,
  ZB_GPIO_EDGE_FALLING,
  ZB_GPIO_EDGE_BOTH
} zb_gpio_edge_t;

typedef struct zb_gpio_conf_s
{
  char path[ZB_GPIO_MAX_FD_PATH_LEN];
#if defined(SUPPORT_GPIO_DEV_GPIOCHIP)
  zb_uint8_t line;
#endif /* SUPPORT_GPIO_DEV_GPIOCHIP */
  int fd;
  zb_gpio_direction_t direction;
  zb_gpio_edge_t edge;
} zb_gpio_conf_t;

/**
  Initialize input GPIO
  Do export and setup direction
  Setup edge in case the EDGE is not NONE

   @param conf    - the structure that is going to be initialized
   @param path    - path of the GPIO
   @param edge    - edge type for the interruption
                    ZB_GPIO_EDGE_NONE in case there's no need to setup the edge

   @return RET_OK in case of success, otherwise the error

 */
zb_ret_t zb_gpio_in_init(zb_gpio_conf_t *conf, const char *path, zb_gpio_edge_t edge);

/**
  Initialize output GPIO
  Do export and setup direction

   @param conf    - the structure that is going to be initialized
   @param path    - path of the pin (can be NULL)

   @return RET_OK in case of success, otherwise the error

 */
zb_ret_t zb_gpio_out_init(zb_gpio_conf_t *conf, const char *path);

/**
  Deinitialize GPIO

   @param conf    - the structure that is going to be deinitialized

   @return nothing

 */
void zb_gpio_deinit(zb_gpio_conf_t *conf);

/**
  Setup direction
  Setup edge in case the EDGE is not NONE

   @param conf    - the structure that is going to be initialized

   @return RET_OK in case of success, otherwise the error

 */
zb_ret_t zb_gpio_setup(zb_gpio_conf_t *conf);

/**
  Write to the GPIO

   @param conf    - the GPIO descriptor
   @param state   - the state of GPIO that should be set
                    in fact the operation of write of
                    the values "0\n" or "1\n" will be performed

   @return RET_OK in case of success, otherwise the error

 */
zb_ret_t zb_gpio_on_off(zb_gpio_conf_t *conf, zb_bool_t state);

zb_ret_t zb_gpio_adv_lock(zb_gpio_conf_t *conf);

int osif_open_master_pty(const char *symlink_name);

/**
   Read GPIO port state
   @param conf    - the GPIO descriptor
   @return 1 or 0 depending on a port state
           Error code on any error
 */
zb_ret_t zb_gpio_read(zb_gpio_conf_t *conf);

#if defined(SUPPORT_GPIO_DEV_GPIOCHIP)
/**
   Ack GPIO Input event

   @param conf    - the GPIO descriptor
 */
void zb_gpio_in_ack(zb_gpio_conf_t *conf);
#endif

/**
   Open and initialize SPI device supposing it is SPIDEV

   @param path - path to device name, like /dev/spidev0.0
   @param mode - SPI mode
   @param speed - max SPI speed
   @param no_cs - true if CS line of SPIDEV is not used (suppose the app manages GPIO as CS line)

   @return file descriptor of open SPI device (>= 0). error of category SYSTEM if error.
 */
osif_ipc_handle_t zb_spi_init(const char *path, zb_uint8_t mode, zb_uint32_t speed, zb_bool_t no_cs);

/**
   Do a single SPI transfer.

   @param fd - file descriptor returned by zb_spi_init()
   @param tx_buf - data to tx. May be NULL
   @param tx_len - transmit data length. If tx_buf is NULL, tx_len must be 0
   @param rx_buf - buffer to rx into. Can be NULL.
   @param rx_len - size os rx_buf. Must be 0 if rx_buf is NULL
   @param speed - sped used for transaction. Must be <= of speed passed to zb_spi_init()
   @param cs_delay - delay between asserting CS and starting SPI transaction. Not used if 0. If non-zero, non_cs passed to zb_spi_init() must be ZB_FALSE.

   @return 0 of ok, error of category SYSTEM if error.
 */
zb_ret_t zb_spi_transfer(osif_ipc_handle_t fd, const uint8_t *tx_buf, size_t tx_len,
                         const uint8_t *rx_buf, size_t rx_len,
                         zb_uint32_t speed, zb_uint32_t cs_delay);

/**
   Add a wake-up signature on SPI frame

   @param fd - file descriptor returned by zb_spi_init()
   @param disable - configure or not the wake-up frame signature

   @return 0 of ok, error of category SYSTEM if error.
 */
zb_ret_t zb_spi_wakeup_frame(osif_ipc_handle_t fd, zb_bool_t disable);

/*! @} */

#endif /* ZB_OSIF_UNIX_H */

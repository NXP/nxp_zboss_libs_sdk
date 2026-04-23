/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
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
/* PURPOSE: OS and platform depenednt stuff for Unix platform
*/

#ifndef ZB_OSIF_ZEPHYR_H
#define ZB_OSIF_ZEPHYR_H 1

/*! \addtogroup ZB_OSIF_ZEPHYR */
/*! @{ */

#include "zb_config.h"

#include "zephyr/autoconf.h"


#include <sys/time.h>
#include <time.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/reboot.h>
#include <ctype.h>
#include <strings.h>

#define ZB_ABORT() \
  do { \
      printk("abort() from %s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); \
    __ASSERT_NO_MSG(1); \
    __ASSERT_NO_MSG(0); \
  } while(0)


#define ZB_SDCC_BANKED

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

/* use macros to be able to redefine */
#define ZB_MALLOC   k_malloc
#define ZB_CALLOC   k_calloc
#define ZB_REALLOC  k_realloc
#define ZB_FREE     k_free

#define ZB_DO_EXIT()  sys_reboot(SYS_REBOOT_WARM)

#define ZB_MEMCPY memcpy
#define ZB_MEMMOVE memmove
#define ZB_MEMSET memset
#define ZB_MEMCMP memcmp
#define ZB_MEMCHR memchr

#define ZB_STRERROR() strerror(errno)

#define ZB_PRINT_OUT(format, ...)  printk(format, ##__VA_ARGS__)
#define ZB_PRINT_ERR(format, ...)  printk(format, ##__VA_ARGS__)

#define ZB_TOUPPER(c) toupper(c)
#define ZB_TOLOWER(c) tolower(c)

#define ZVUNUSED(v) (void)v

#ifdef ZB_STACK_REGRESSION_TESTING_API
#define ZB_ENABLE_ALL_INTER()  ZB_REGRESSION_TESTS_API().osif_interrupts_balance++
#define ZB_DISABLE_ALL_INTER() ZB_REGRESSION_TESTS_API().osif_interrupts_balance--
#else
#define ZB_ENABLE_ALL_INTER()
#define ZB_DISABLE_ALL_INTER()
#endif /* ZB_STACK_REGRESSION_TESTING_API */


#define ZB_VOLATILE volatile

#define ZB_BZERO(s,l) ZB_MEMSET((char*)(s), 0, (l))
#define ZB_BZERO2(s) ZB_BZERO(s, 2)

void zb_zephyr_platform_init(void);
void zb_osif_init_timer(void);

#define ZB_PLATFORM_INIT() zb_zephyr_platform_init()


#define ZB_TIMER_INIT() zb_osif_init_timer()
#define ZB_CHECK_TIMER_IS_ON() 1 /*!< always on in linux */
#define ZB_START_HW_TIMER() /*!< nothing to do here */

typedef int osif_ipc_handle_t;
typedef void* osif_wait_control_t;

void osif_ipc_init(void);
void osif_ipc_deinit(void);
void zb_scheduler_wakeup(void);

#ifdef MAC_TRANSPORT_USES_SELECT
void zb_osif_platform_io_iteration(zb_bool_t block);
#else
void zb_zephyr_go_idle(void);
#endif

#if defined ZB_COMPILE_MAC_MONOLITHIC
#define ZB_TRANSPORT_BLOCK() zb_nsng_io_iteration(ZB_TRUE)
#define ZB_TRANSPORT_NONBLOCK_ITERATION() zb_nsng_io_iteration(ZB_FALSE)
#else
#define ZB_TRANSPORT_BLOCK() zb_osif_platform_io_iteration(ZB_TRUE)
#define ZB_TRANSPORT_NONBLOCK_ITERATION() zb_osif_platform_io_iteration(ZB_FALSE)
#endif /* ZB_COMPILE_MAC_MONOLITHIC */


void zb_zephyr_platform_init(void);


#ifndef CONFIG_NEWLIB_LIBC
/* Avoid loading libc & posix */
#ifndef _TIME_H_
struct tm
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
};
#endif

struct tm *localtime_r 	(const time_t *__restrict, struct tm *__restrict);
#endif /* CONFIG_NEWLIB_LIBC */

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

void osif_sleep(zb_uint_t s);
void osif_usleep(zb_uint_t us);
void zb_osif_goto_idle(void);
#define zb_osif_low_power_config(enable) zephyr_low_power_config(enable)

void zephyr_low_power_config(uint8_t enable);
void zephyr_low_power_off(void);
void zephyr_low_power_on(void);

uint8_t zephyr_ota_get_partition_id(const char *partition_name);
const struct device *zephyr_ota_get_device_pointer(const char *partition_name, uint32_t *offset, uint32_t *size);

void zb_osif_prod_cfg_init(void);
void zb_osif_prod_cfg_deinit(void);

/* threads */
typedef struct k_mutex   osif_mutex_t;
typedef struct k_thread  osif_thread_t;
typedef k_thread_stack_t osif_thread_stack_t;
typedef void *osif_func_ret_t;
typedef void *osif_func_arg_t;
typedef osif_func_ret_t (*osif_func_t)(osif_func_arg_t);
typedef osif_func_ret_t (*osif_tread_detached_func_t)(osif_func_arg_t);

#ifdef CONFIG_USERNAME
#define OSIF_THREAD_STACK_DEFINE(stack_name, stack_size)  static K_KERNEL_STACK_DEFINE(stack_name, stack_size)
#define OSIF_THREAD_STACK_SIZE(stack_name)                K_KERNEL_STACK_SIZEOF(stack_name)
#else
#define OSIF_THREAD_STACK_DEFINE(stack_name, stack_size)  static K_THREAD_STACK_DEFINE(stack_name, stack_size)
#define OSIF_THREAD_STACK_SIZE(stack_name)                K_THREAD_STACK_SIZEOF(stack_name)
#endif

#define   osif_start_thread(thread, func, arg, stack_name, stack_size, thread_priority) RET_NOT_SUPPORTED
zb_ret_t   osif_start_thread2(osif_thread_t *thread, osif_func_t func, void * arg, osif_thread_stack_t *stack_name, size_t stack_size, zb_int_t thread_priority);
void* osif_thread_join(osif_thread_t *thread);
zb_uint_t  osif_get_thread_id(ZB_VOID_ARGLIST);
void       osif_thread_exit(void *retval);

zb_ret_t osif_init_mutex(osif_mutex_t *mutex);
zb_ret_t osif_deinit_mutex(osif_mutex_t *mutex);
zb_ret_t osif_lock_mutex(osif_mutex_t *mutex);
zb_ret_t osif_trylock_mutex(osif_mutex_t *mutex);
zb_ret_t osif_unlock_mutex(osif_mutex_t *mutex);

void osif_sched_yield();

void zb_osif_update_timer(void);

#define ZB_CHECK_TIMER_IS_ON() 1 /*!< always on in linux */
#define ZB_START_HW_TIMER() /*!< nothing to do here */
#define ZB_STOP_HW_TIMER() /*!< nothing to do here */

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


#endif /* ZB_OSIF_ZEPHYR_H */

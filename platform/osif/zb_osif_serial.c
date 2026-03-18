/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * Copyright 2023-2026 NXP
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
/* PURPOSE: serial transport, common
*/

#define ZB_TRACE_FILE_ID 34016
#include "zboss_api_core.h"

#include <zephyr/drivers/uart.h>

#undef SEND_ZBOSS_LOGS_ON_UART

#ifdef SEND_ZBOSS_LOGS_ON_UART
const struct device *uart_rtos_dev = DEVICE_DT_GET(DT_NODELABEL(lpuart0));

struct uart_config uart_rtos_cfg = {
  .baudrate = 115200,
  .parity = UART_CFG_PARITY_NONE,
  .stop_bits = UART_CFG_STOP_BITS_1,
  .data_bits = UART_CFG_DATA_BITS_8,
  .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
};
#endif /* SEND_ZBOSS_LOGS_ON_UART */

void zb_osif_serial_init(void)
{
#ifdef SEND_ZBOSS_LOGS_ON_UART
  if (!device_is_ready(uart_rtos_dev)) {
    WCS_TRACE_ERROR("%s() uart not ready", __FUNCTION__);
    return;
  }

  if (uart_configure(uart_rtos_dev, &uart_rtos_cfg) != 0) {
    WCS_TRACE_ERROR("%s() uart config failed", __FUNCTION__);
    return;
  }
#endif /* SEND_ZBOSS_LOGS_ON_UART */
}

void zb_osif_serial_put_bytes(const zb_uint8_t *buf, zb_short_t len)
{
#ifndef SEND_ZBOSS_LOGS_ON_UART
  ZVUNUSED(buf);
  ZVUNUSED(len);
#else /* SEND_ZBOSS_LOGS_ON_UART */
  if(!len || !buf)
    return;

  for(zb_short_t i=0; i<len; i++)
    uart_poll_out(uart_rtos_dev, buf[i]);
#endif /* SEND_ZBOSS_LOGS_ON_UART */
}

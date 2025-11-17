/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
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
/* PURPOSE: 
*/

#ifndef ZB_NCP_SERIAL_H
#define ZB_NCP_SERIAL_H 1

#include "zb_config.h"

#if defined(ZB_NCP_TRANSPORT_TYPE_SERIAL) || defined(ZB_NCP_TRANSPORT_TYPE_USERIAL)

const char* zb_ncp_uart_path(void);

#endif /* ZB_NCP_TRANSPORT_TYPE_SERIAL || ZB_NCP_TRANSPORT_TYPE_USERIAL */

#endif /* ZB_NCP_SERIAL_H */

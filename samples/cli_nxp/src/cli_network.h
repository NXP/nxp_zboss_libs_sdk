/*
 * Copyright 2024 NXP
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

#ifndef CLI_NXP_NETWORK_H
#define CLI_NXP_NETWORK_H 1

#include "zboss_api.h"
#include "cli_menu.h"


extern cli_menu_cmd menu_network[];

#if defined(ZB_ROUTER_ROLE) || defined(ZB_ED_ROLE)
zb_ret_t network_do_join(zb_bool_t distributed);
#endif

#endif /* CLI_NXP_NETWORK_H */


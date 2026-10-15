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

#ifndef CLI_NXP_NVRAM_H
#define CLI_NXP_NVRAM_H 1

#include "zboss_api.h"
#include "cli_menu.h"

extern cli_menu_cmd menu_nvram[];

/*static */zb_ret_t nvram_erase_at_start(int argc, char *argv[]); /* Shared with config factory_reset */

#endif /* CLI_NXP_NVRAM_H */


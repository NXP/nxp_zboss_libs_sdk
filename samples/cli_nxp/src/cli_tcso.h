/*
 * Copyright 2026 NXP
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

#ifndef CLI_NXP_TCSO_H
#define CLI_NXP_TCSO_H 1

#include "zboss_api.h"
#include "cli_menu.h"

/* TC Swap Out (TCSO) CLI menu.
 * Only meaningful on a Coordinator build (TC role). On ZED/ZR builds the menu
 * is still registered but the commands return RET_UNAUTHORIZED / RET_NOT_IMPLEMENTED.
 */
extern cli_menu_cmd menu_tcso[];

/* Hooks to be called from cli_main.c zboss_signal_handler() so the TCSO
 * backup/restore state machine is driven by the stack signals.
 *
 * tcso_on_signal() must be called for EVERY signal received by the app.
 * It handles:
 *   - ZB_TCSWAP_DB_BACKUP_REQUIRED_SIGNAL : auto (re)backup when TC DB is dirty
 *   - ZB_ZDO_SIGNAL_SKIP_STARTUP          : perform a pending restore + optional
 *                                           ZC-identity change, before formation
 *   - ZB_TC_SWAPPED_SIGNAL                : (ZR/ZED) mark that TC swap-out rejoin
 *                                           has completed
 *   - ZB_TCLK_UPDATED_SIGNAL              : (ZR/ZED) mark TCLK re-negotiated
 *
 * Returns ZB_TRUE if the signal was consumed by TCSO (informational only; the
 * caller may still process it).
 */
zb_bool_t tcso_on_signal(zb_zdo_app_signal_type_t sig, zb_zdo_app_signal_hdr_t *sg_p, zb_ret_t status);

/* TCSO feature master switch. TCSO is disabled by default; the operator turns
 * it on with "tcso enable 1". Returns ZB_TRUE when the feature is enabled.
 * cli_main.c uses this to avoid calling tcso_on_signal() while disabled. */
zb_bool_t tcso_is_enabled(void);

/* Called once at init to register the NVRAM app-dataset callbacks used to
 * persist / reload the TC DB (ZB_NVRAM_APP_DATA1). Safe to call on any role. */
void tcso_init(void);

#endif /* CLI_NXP_TCSO_H */

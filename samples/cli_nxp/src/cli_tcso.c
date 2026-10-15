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
 * PURPOSE: Trust Center Swap Out (TCSO) CLI menu.
 *
 * Typical two-window flow (see test-example-cli_nxp.txt, SEQUENCE FOR TRUST
 * CENTER SWAP OUT):
 *   ZC1 window : config+form, ZED joins, "tcso backup"                (backup TC DB to NVRAM)
 *                ... power down ZC1 ...
 *   ZC2 window : "tcso restore_at_start 1" then reboot/config start   (restore + change ZC identity)
 *   ZED window : follows the TC swap-out rejoin automatically; verify with "tcso status"
 */

#define ZB_TRACE_FILE_ID 60040
#include "zboss_api.h"
#include "cli_tcso.h"
#include "cli_config.h"
#include "cli_menu.h"
#include "cli_tools.h"

#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)
#include "zboss_tcswap.h"
#endif

/* -------------------------------------------------------------------------- */
/* Tunables (kept aligned with the tc_swap_out sample)                        */
/* -------------------------------------------------------------------------- */
#define TCSW_NVRAM_DATASET_SIZE 1024U
#define TCSW_PORTION_SIZE       32U

/* Default "new ZC2 identity" applied on restore when the operator asks to
 * simulate a distinct replacement TC */
#define TCSO_DEFAULT_NEW_PANID   0xABABU
static const zb_ieee_addr_t TCSO_DEFAULT_NEW_IEEE =
  { 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab };
static const zb_uint8_t     TCSO_DEFAULT_NEW_NWK_KEY[16] =
  { 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
    0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab };

/* -------------------------------------------------------------------------- */
/* Module state                                                               */
/* -------------------------------------------------------------------------- */
/* Feature master switch. TCSO is OFF by default: the tcso commands (except
 * "tcso enable") are rejected and tcso_on_signal() does nothing until the
 * operator explicitly runs "tcso enable 1". */
static zb_bool_t   g_tcso_enabled   = ZB_FALSE;

static zb_uint8_t  g_nvram_buf[TCSW_NVRAM_DATASET_SIZE] = {0};
static zb_bool_t   g_tc_db_loaded   = ZB_FALSE;   /* set by NVRAM read cb: a saved TC DB is available */

/* Restore-at-start request, consumed on ZB_ZDO_SIGNAL_SKIP_STARTUP */
static zb_bool_t   g_restore_pending = ZB_FALSE;
static zb_bool_t   g_change_identity = ZB_FALSE;  /* apply new PANID/IEEE/nwk key after restore */
static zb_bool_t   g_change_tc_addr  = ZB_FALSE;  /* pass-through to zb_tcsw_start_restore_db()  */

/* Overridable "new ZC2 identity" */
static zb_uint16_t   g_new_panid = TCSO_DEFAULT_NEW_PANID;
static zb_ieee_addr_t g_new_ieee;
static zb_uint8_t     g_new_nwk_key[ZB_CCM_KEY_SIZE];
static zb_bool_t      g_identity_overridden = ZB_FALSE;

/* ZR/ZED observation (only usable when built as a router; harmless otherwise) */
static zb_bool_t   g_tc_swapped   = ZB_FALSE;
static zb_bool_t   g_tclk_updated = ZB_FALSE;

/* -------------------------------------------------------------------------- */
/* Command handlers                                                           */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_enable(int argc, char *argv[]);            static zb_ret_t help_enable(void);
static zb_ret_t tcso_status(int argc, char *argv[]);
static zb_ret_t tcso_backup(int argc, char *argv[]);            static zb_ret_t help_backup(void);
static zb_ret_t tcso_restore_at_start(int argc, char *argv[]);  static zb_ret_t help_restore_at_start(void);
static zb_ret_t tcso_change_id(int argc, char *argv[]);         static zb_ret_t help_change_id(void);
static zb_ret_t tcso_passkey_backup(int argc, char *argv[]);    static zb_ret_t help_passkey_backup(void);

cli_menu_cmd menu_tcso[] = {
  /* name, args,                              align, function,             help,                   description */
  { "enable", " <0|1>", "                          ", tcso_enable,          help_enable,            "enable [1] / disable [0] the whole TCSO feature (default: disabled)" },
  { "status", "", "                                ", tcso_status,          help_empty,             "print TCSO state: db size, need_backup, busy, saved DB, tc_swapped, tclk_updated" },
  { "backup", "", "                                ", tcso_backup,          help_backup,            "backup the TC database now and persist it to NVRAM (ZC/TC only)" },
  { "restore_at_start", " [chg_addr]", "           ", tcso_restore_at_start,help_restore_at_start,  "on next stack start, restore TC DB from NVRAM; chg_addr [0|1] lets ZC2 use a different IEEE" },
  { "change_id", " <panid> <IEEE> <KEY128>", "     ", tcso_change_id,       help_change_id,         "set the ZC2 replacement identity applied after restore: panid [0x0-0xFFFF], IEEE, 16-byte nwk key" },
  { "passkey_backup", " [enable]", "               ", tcso_passkey_backup,  help_passkey_backup,    "enable/disable backup of the DLK authentication token: [0|1] (default 1)" },
  /* Add new commands above here */
  { NULL, NULL, NULL, NULL, NULL, NULL }
};

/* -------------------------------------------------------------------------- */
/* NVRAM app-dataset callbacks (persist / reload the TC DB blob)              */
/* Mirror of the tc_swap_out sample: dataset ZB_NVRAM_APP_DATA1.              */
/* -------------------------------------------------------------------------- */
#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE) && defined(ZB_USE_NVRAM)

static zb_ret_t tcso_nvram_write_cb(zb_uint8_t page, zb_uint32_t pos)
{
  zb_ret_t ret = zb_nvram_write_data(page, pos, g_nvram_buf, TCSW_NVRAM_DATASET_SIZE);
  menu_printf("tcso_nvram_write_cb(): %u bytes -> %s", TCSW_NVRAM_DATASET_SIZE, wcs_get_error_str(ret));
  return ret;
}

static void tcso_nvram_read_cb(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length)
{
  ZVUNUSED(payload_length);
  zb_nvram_read_data(page, pos, g_nvram_buf, TCSW_NVRAM_DATASET_SIZE);
  g_tc_db_loaded = ZB_TRUE;
  menu_printf("tcso_nvram_read_cb(): loaded saved TC DB (%u bytes)", TCSW_NVRAM_DATASET_SIZE);
}

static zb_uint16_t tcso_nvram_size_cb(void)
{
  return TCSW_NVRAM_DATASET_SIZE;
}

#endif /* TC_SWAPOUT && ZB_COORDINATOR_ROLE && ZB_USE_NVRAM */

/* -------------------------------------------------------------------------- */
/* Public init                                                                */
/* -------------------------------------------------------------------------- */
void tcso_init(void)
{
  ZB_MEMCPY(g_new_ieee, TCSO_DEFAULT_NEW_IEEE, sizeof(zb_ieee_addr_t));
  ZB_MEMCPY(g_new_nwk_key, TCSO_DEFAULT_NEW_NWK_KEY, sizeof(g_new_nwk_key));

#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE) && defined(ZB_USE_NVRAM)
  /* Register the app1 dataset so the TC DB survives a power cycle / can be
   * copied to ZC2. In production the backup should go to external storage. */
  zb_nvram_register_app1_read_cb(tcso_nvram_read_cb);
  zb_nvram_register_app1_write_cb(tcso_nvram_write_cb, tcso_nvram_size_cb);
#endif
}

zb_bool_t tcso_is_enabled(void)
{
  return g_tcso_enabled;
}

/* -------------------------------------------------------------------------- */
/* Backup / restore engines                                                   */
/* -------------------------------------------------------------------------- */
#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)

static zb_ret_t tcso_do_backup(void)
{
  zb_uint_t portion_len;
  zb_uint_t pos = 0U;
  zb_uint_t db_size;
  zb_ret_t  ret;

  db_size = zb_tcsw_calculate_db_size();
  if (!zb_tcsw_check_buffer_size(db_size) || db_size > TCSW_NVRAM_DATASET_SIZE)
  {
    menu_printf("tcso backup: DB size %u exceeds buffer %u", db_size, (zb_uint_t)TCSW_NVRAM_DATASET_SIZE);
    return RET_NO_MEMORY;
  }

  ret = zb_tcsw_start_backup_db();
  if (ret != RET_OK)
  {
    menu_printf("tcso backup: zb_tcsw_start_backup_db() failed: %s", wcs_get_error_str(ret));
    return ret;
  }

  do
  {
    portion_len = zb_tcsw_get_portion(g_nvram_buf + pos, TCSW_PORTION_SIZE);
    pos += portion_len;
  }
  while (portion_len != 0U && pos < TCSW_NVRAM_DATASET_SIZE);

  menu_printf("tcso backup: collected %u bytes", pos);

#if defined(ZB_USE_NVRAM)
  ret = zb_nvram_write_dataset(ZB_NVRAM_APP_DATA1);
  menu_printf("tcso backup: persist to NVRAM -> %s", wcs_get_error_str(ret));
#else
  menu_printf("tcso backup: NVRAM not enabled, DB kept only in RAM");
#endif

  return ret;
}

static zb_ret_t tcso_do_restore(zb_bool_t change_tc_addr)
{
  zb_ret_t  ret;
  zb_uint_t off = 0U;

  if (!g_tc_db_loaded)
  {
    menu_printf("tcso restore: no saved TC DB loaded from NVRAM");
    return RET_NOT_FOUND;
  }

  ret = zb_tcsw_start_restore_db(change_tc_addr);
  if (ret != RET_OK)
  {
    menu_printf("tcso restore: zb_tcsw_start_restore_db() failed: %s", wcs_get_error_str(ret));
    return ret;
  }

  do
  {
    ret = zc_tcsw_restore_portion(g_nvram_buf + off, TCSW_PORTION_SIZE);
    off += TCSW_PORTION_SIZE;
  }
  while (ret == RET_AGAIN && off < TCSW_NVRAM_DATASET_SIZE);

  if (ret != RET_OK)
  {
    menu_printf("tcso restore: restore_portion failed at %u: %s", off, wcs_get_error_str(ret));
    return ret;
  }

  ret = zc_tcsw_restore_finish();
  menu_printf("tcso restore: finished (%u bytes) -> %s", off, wcs_get_error_str(ret));
  return ret;
}

static void tcso_apply_new_identity(void)
{
  ZB_PIBCACHE_PAN_ID() = g_new_panid;
  zb_set_long_address((zb_uint8_t *)g_new_ieee);
  zb_secur_setup_nwk_key(g_new_nwk_key, 0);
  menu_printf("tcso: applied ZC2 identity panid 0x%04x (new IEEE + nwk key set)", g_new_panid);
}

#endif /* TC_SWAPOUT && ZB_COORDINATOR_ROLE */

/* -------------------------------------------------------------------------- */
/* Signal hook (called from cli_main.c zboss_signal_handler)                  */
/* -------------------------------------------------------------------------- */
zb_bool_t tcso_on_signal(zb_zdo_app_signal_type_t sig, zb_zdo_app_signal_hdr_t *sg_p, zb_ret_t status)
{
  ZVUNUSED(sg_p);
  ZVUNUSED(status);

  /* TCSO is OFF by default: do nothing until "tcso enable 1" is run. */
  if (g_tcso_enabled != ZB_TRUE)
  {
    return ZB_FALSE;
  }

  switch (sig)
  {
#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
      /* This is emitted by zboss_start_no_autostart() before formation.
       * If the operator armed a restore, do it here then continue startup. */
      if (g_restore_pending)
      {
        menu_printf("tcso: SKIP_STARTUP -> performing pending restore");
        (void)tcso_do_restore(g_change_tc_addr);
        if (g_change_identity)
        {
          tcso_apply_new_identity();
        }
        g_restore_pending = ZB_FALSE;
      }
      /* NOTE: cli_main.c must NOT swallow SKIP_STARTUP; the state machine in
       * cli_main.c already drives zboss_start_no_autostart()/continue via its
       * own STATE_RUN loop, so we do not call zboss_start_continue() here. */
      return ZB_TRUE;

    case ZB_TCSWAP_DB_BACKUP_REQUIRED_SIGNAL:
      /* Stack tells us the TC DB became dirty (new device joined / key change).
       * Auto-refresh the backup so a subsequent restore is up to date. */
      menu_printf("tcso: ZB_TCSWAP_DB_BACKUP_REQUIRED_SIGNAL (need_backup=%u) -> auto backup",
                  zb_tcsw_need_backup());
      (void)tcso_do_backup();
      return ZB_TRUE;
#endif /* TC_SWAPOUT && ZB_COORDINATOR_ROLE */

    case ZB_TC_SWAPPED_SIGNAL:
      g_tc_swapped = ZB_TRUE;
      menu_printf("tcso: ZB_TC_SWAPPED_SIGNAL received (TC swap-out rejoin done)");
      return ZB_TRUE;

    case ZB_TCLK_UPDATED_SIGNAL:
      g_tclk_updated = ZB_TRUE;
      menu_printf("tcso: ZB_TCLK_UPDATED_SIGNAL received (link key re-negotiated)");
      return ZB_TRUE;

    default:
      break;
  }

  return ZB_FALSE;
}

/* -------------------------------------------------------------------------- */
/* tcso enable <0|1>                                                          */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_enable(int argc, char *argv[])
{
  zb_ret_t   ret;
  zb_uint8_t enable;

  if (argc != 1)
    return RET_INVALID_PARAMETER;

  TOOLS_GET_ARG(ret, uint8, argv, 0, &enable);
  if (enable > 1)
    return RET_INVALID_PARAMETER_1;

  g_tcso_enabled = (enable != 0) ? ZB_TRUE : ZB_FALSE;
  menu_printf("tcso: feature %s", g_tcso_enabled ? "enabled" : "disabled");
  return RET_OK;
}
static zb_ret_t help_enable(void)
{
  menu_printf("Enable [1] or disable [0] the whole TCSO feature.");
  menu_printf("Disabled by default: while disabled, all other tcso commands are");
  menu_printf("rejected and the stack signals are not processed by TCSO.");
  return RET_OK;
}

/* -------------------------------------------------------------------------- */
/* tcso status                                                                */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_status(int argc, char *argv[])
{
  ZVUNUSED(argv);
  if (argc != 0)
    return RET_INVALID_PARAMETER;

  menu_printf("  enabled        : %u", g_tcso_enabled);
#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)
  menu_printf("TCSO (coordinator build):");
  menu_printf("  db_size        : %u", zb_tcsw_calculate_db_size());
  menu_printf("  need_backup    : 0x%x (bit0=SECUR bit1=BINDINGS)", zb_tcsw_need_backup());
  menu_printf("  busy           : %u", zb_tcsw_is_busy());
#else
  menu_printf("TCSO: not a coordinator/TC build (TC_SWAPOUT & ZB_COORDINATOR_ROLE required for backup/restore)");
#endif
  menu_printf("  saved DB loaded: %u", g_tc_db_loaded);
  menu_printf("  restore_pending: %u (chg_id=%u chg_tc_addr=%u)", g_restore_pending, g_change_identity, g_change_tc_addr);
  menu_printf("  new panid      : 0x%04x%s", g_new_panid, g_identity_overridden ? " (overridden)" : " (default)");
  menu_printf("  tc_swapped     : %u", g_tc_swapped);
  menu_printf("  tclk_updated   : %u", g_tclk_updated);

  return RET_OK;
}

/* -------------------------------------------------------------------------- */
/* tcso backup                                                                */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_backup(int argc, char *argv[])
{
  ZVUNUSED(argv);
  if (argc != 0)
    return RET_INVALID_PARAMETER;

  if (!g_tcso_enabled)
  {
    menu_printf("tcso backup: feature disabled, run 'tcso enable 1' first");
    return RET_UNAUTHORIZED;
  }

#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)
  if (!config_is_started())
    return RET_UNAUTHORIZED;
  if (!config_is_coordinator())
    return RET_UNAUTHORIZED;

  return tcso_do_backup();
#else
  menu_printf("tcso backup: requires TC_SWAPOUT + ZB_COORDINATOR_ROLE build");
  return RET_NOT_IMPLEMENTED;
#endif
}
static zb_ret_t help_backup(void)
{
  menu_printf("Snapshot the TC database (link keys, frame counters, nwk key,");
  menu_printf("bindings, DLK auth token) and persist it to NVRAM app dataset 1.");
  menu_printf("Run on ZC1 once the DUT has joined, BEFORE powering ZC1 down.");
  return RET_OK;
}

/* -------------------------------------------------------------------------- */
/* tcso restore_at_start [chg_addr]                                           */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_restore_at_start(int argc, char *argv[])
{
  zb_ret_t   ret = RET_OK;
  zb_uint8_t chg_addr = 0;

  if (!g_tcso_enabled)
  {
    menu_printf("tcso restore_at_start: feature disabled, run 'tcso enable 1' first");
    return RET_UNAUTHORIZED;
  }

#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)
  if (argc > 1)
    return RET_INVALID_PARAMETER;

  if (argc == 1)
  {
    TOOLS_GET_ARG(ret, uint8, argv, 0, &chg_addr);
    if (chg_addr > 1)
      return RET_INVALID_PARAMETER_1;
  }

  if (config_is_started())
  {
    menu_printf("tcso restore_at_start: stop the stack first (config stop), then arm restore and (re)start");
    return RET_UNAUTHORIZED;
  }

  g_restore_pending = ZB_TRUE;
  g_change_tc_addr  = (chg_addr != 0) ? ZB_TRUE : ZB_FALSE;
  g_change_identity = ZB_TRUE; /* ZC2 must look different -> apply new panid/IEEE/nwk key */

  menu_printf("tcso restore_at_start: armed (chg_tc_addr=%u). Now run 'config start' to restore & take over as new TC.", chg_addr);
  return RET_OK;
#else
  ZVUNUSED(argv);
  ZVUNUSED(chg_addr);
  ZVUNUSED(ret);
  menu_printf("tcso restore_at_start: requires TC_SWAPOUT + ZB_COORDINATOR_ROLE build");
  return RET_NOT_IMPLEMENTED;
#endif
}
static zb_ret_t help_restore_at_start(void)
{
  menu_printf("Arm a TC-DB restore to be performed on the NEXT stack start.");
  menu_printf("Use on ZC2: load the DB saved by ZC1 (same NVRAM image / copied),");
  menu_printf("then 'config start'. ZC2 restores the DB and takes over as the TC.");
  menu_printf("chg_addr=1 makes ZC2 keep a different IEEE.");
  return RET_OK;
}

/* -------------------------------------------------------------------------- */
/* tcso change_id <panid> <IEEE> <KEY128>                                     */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_change_id(int argc, char *argv[])
{
  zb_ret_t       ret;
  zb_uint16_t    new_panid;
  zb_ieee_addr_t new_ieee = { 0 };
  zb_uint8_t     new_key[ZB_CCM_KEY_SIZE];

  if (!g_tcso_enabled)
  {
    menu_printf("tcso change_id: feature disabled, run 'tcso enable 1' first");
    return RET_UNAUTHORIZED;
  }

  if (argc != 3)
    return RET_INVALID_PARAMETER;

  /* [panid] */
  TOOLS_GET_ARG(ret, uint16, argv, 0, &new_panid);

  /* [IEEE] (the ieee getter fills the array directly) */
  TOOLS_GET_ARG_HEXA(ret, ieee, argv, 1, new_ieee);

  /* [KEY128] : 16-byte network key */
  TOOLS_GET_ARG_HEXA(ret, nwk_key, argv, 2, new_key);

  g_new_panid = new_panid;
  ZB_MEMCPY(g_new_ieee, new_ieee, sizeof(zb_ieee_addr_t));
  ZB_MEMCPY(g_new_nwk_key, new_key, ZB_CCM_KEY_SIZE);
  g_identity_overridden = ZB_TRUE;

  menu_printf("tcso change_id: ZC2 identity set (panid 0x%04x)", g_new_panid);
  return RET_OK;
}
static zb_ret_t help_change_id(void)
{
  menu_printf("Override the replacement-TC (ZC2) identity applied after restore.");
  menu_printf("Defaults: panid 0xABAB, IEEE ab:..:ab, nwk key ab:..:ab.");
  return RET_OK;
}

/* -------------------------------------------------------------------------- */
/* tcso passkey_backup [enable]                                               */
/* -------------------------------------------------------------------------- */
static zb_ret_t tcso_passkey_backup(int argc, char *argv[])
{
  zb_ret_t   ret;
  zb_uint8_t enable = 1;

  if (!g_tcso_enabled)
  {
    menu_printf("tcso passkey_backup: feature disabled, run 'tcso enable 1' first");
    return RET_UNAUTHORIZED;
  }

#if defined(TC_SWAPOUT) && defined(ZB_COORDINATOR_ROLE)
  if (argc != 1)
    return RET_INVALID_PARAMETER;

  TOOLS_GET_ARG(ret, uint8, argv, 0, &enable);
  if (enable > 1)
    return RET_INVALID_PARAMETER_1;

  zb_tcsw_enable_passkey_backup(enable != 0 ? ZB_TRUE : ZB_FALSE);
  menu_printf("tcso passkey_backup: %s", enable ? "enabled" : "disabled");
  return RET_OK;
#else
  ZVUNUSED(argv);
  ZVUNUSED(enable);
  ZVUNUSED(ret);
  menu_printf("tcso passkey_backup: requires TC_SWAPOUT + ZB_COORDINATOR_ROLE build");
  return RET_NOT_IMPLEMENTED;
#endif
}
static zb_ret_t help_passkey_backup(void)
{
  menu_printf("Include/exclude the DLK authentication token in the TC backup.");
  menu_printf("Default: enabled.");
  return RET_OK;
}

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

#include <zephyr/kernel.h>
#ifdef CONFIG_PM
#include <zephyr/pm/policy.h>
#include <zephyr/pm/state.h>
#endif

static uint8_t lp_config = true;
#ifdef CONFIG_PM
static uint8_t lp_on = true; /* Avoid unbalanced state lock get/put */
#endif

void zephyr_low_power_off(void);
void zephyr_low_power_on(void);

void zephyr_low_power_config(uint8_t enable)
{
  lp_config = true; /* force turn off low power */
  zephyr_low_power_off();
  lp_config = enable;
}

void zephyr_low_power_off(void)
{
#ifdef CONFIG_PM
  /* Prevent system from entering low power states */
  if(lp_config && lp_on) pm_policy_state_all_lock_get();
  lp_on = false;
#endif
}

void zephyr_low_power_on(void)
{
#ifdef CONFIG_PM
  /* Allow system to enter low power states */
  if(lp_config && !lp_on) pm_policy_state_all_lock_put();
  lp_on = true;
#endif
}

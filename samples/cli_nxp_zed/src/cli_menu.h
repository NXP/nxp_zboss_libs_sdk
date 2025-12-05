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

#ifndef CLI_NXP_MENU_H
#define CLI_NXP_MENU_H 1

#include "zboss_api.h"

#undef CLI_DEBUG

typedef zb_ret_t (*cli_menu_fct)(int argc, char *argv[]);
typedef zb_ret_t (*cli_help_fct)(void);

typedef struct {
  char         *name;   /* name of the command */
  char         *args;   /* list of args */
  char         *align;  /* spaces to add to align description */
  cli_menu_fct func;    /* function to call */
  cli_help_fct help;    /* function to help */
  char         *descr;  /* description */
} cli_menu_cmd;


zb_ret_t menu_init(void);
void menu_run(void);
void menu_add_history(char *line);
void menu_shutdown(void);

void menu_printf(const char *format , ...);

zb_ret_t help_empty(void);

void menu_cb_occured(void);


#endif /* CLI_NXP_MENU_H */


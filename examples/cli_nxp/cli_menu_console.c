/*
 * Copyright 2024-2025 NXP
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

#include "cli_menu.c"


zb_ret_t menu_init(void)
{
  return zb_osif_readline_init();
}

void menu_run(void)
{
  while(!osif_is_term_sig_received())
  {
    char *line;

    line = zb_osif_readline_get("zbcli> ");

    if(line && strlen(line) > 0)
    {
      char *savep;
      char *p = strtok_r(line, "\n", &savep);
      while(p)
      {
        menu_process_line(p);
        p = strtok_r(0, "\n", &savep);
      }
    }

    zb_osif_readline_free(line);
  }
  menu_printf("signal SIGTERM received, exiting...");
  osif_thread_exit(NULL);
}

void menu_add_history(char *line)
{
  zb_osif_readline_add_history(line);
}

void menu_shutdown(void)
{
  /* Nothing to do */
}

void menu_printf(const char *format , ...)
{
  va_list args;
  char msgStr[1024];

  va_start (args, format);
  vsnprintf(msgStr, 1024, format, args);
  va_end (args);

  ZB_PRINT_OUT("%s\r\n", msgStr);
}


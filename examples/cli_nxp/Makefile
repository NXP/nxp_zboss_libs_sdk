#/* ZBOSS Zigbee software protocol stack
# *
# * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
# * www.dsr-zboss.com
# * www.dsr-corporation.com
# * All rights reserved.
# *
# * Copyright 2024-2025 NXP
# *
# * This is unpublished proprietary source code of DSR Corporation
# * The copyright notice does not evidence any actual or intended
# * publication of such source code.
# *
# * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
# * Corporation
# *
# * Commercial Usage
# * Licensees holding valid DSR Commercial licenses may use
# * this file in accordance with the DSR Commercial License
# * Agreement provided with the Software or, alternatively, in accordance
# * with the terms contained in a written agreement between you and
# * DSR.
#

IMX_IPADDR ?= 192.168.0.99

ROOT_DIR=../..
include $(ROOT_DIR)/template/Options


APP_CLI_NXP_ZCZR=cli_nxp_zczr
APP_CLI_NXP_ZED=cli_nxp_zed

BINS_CLI_NXP=$(APP_CLI_NXP_ZCZR) $(APP_CLI_NXP_ZED)


APP_ZB_DAEMON_ZCZR=zb_daemon_zczr
APP_ZB_DAEMON_ZED=zb_daemon_zed

BINS_ZB_DAEMON=$(APP_ZB_DAEMON_ZCZR) $(APP_ZB_DAEMON_ZED)


APP_ZB_CTL=zb_ctl

BINS_ZB_CTL=$(APP_ZB_CTL)

.PHONY: all
all: $(BINS_CLI_NXP) $(BINS_ZB_DAEMON) $(BINS_ZB_CTL)


.PHONY: clean
clean:
	@rm -f *.o *.d *.map $(BINS)


.PHONY: deploy help
# => refer to $(ROOT_DIR)/template/Options


#----------  cli_nxp ----------

OBJS_CLI_NXP_ZCZR = cli_main.o cli_menu_console.o cli_config.o cli_network.o cli_endpoint.o cli_cluster.o cli_zdo.o cli_installcode.o cli_nvram.o cli_tools.o
OBJS_CLI_NXP_ZED = cli_main.ed.o cli_menu_console.ed.o cli_config.ed.o cli_network.ed.o cli_endpoint.ed.o cli_cluster.ed.o cli_zdo.ed.o cli_installcode.ed.o cli_nvram.ed.o cli_tools.ed.o

$(APP_CLI_NXP_ZCZR): $(OBJS_CLI_NXP_ZCZR) $(LIBS_ZC)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS) -lreadline -lpthread

$(APP_CLI_NXP_ZED): $(OBJS_CLI_NXP_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS) -lreadline -lpthread


#----------  zb_daemon ----------

OBJS_ZB_DAEMON_ZCZR = cli_main.o cli_menu_socket.o cli_config.o cli_network.o cli_endpoint.o cli_cluster.o cli_zdo.o cli_installcode.o cli_nvram.o cli_tools.o
OBJS_ZB_DAEMON_ZED = cli_main.ed.o cli_menu_socket.ed.o cli_config.ed.o cli_network.ed.o cli_endpoint.ed.o cli_cluster.ed.o cli_zdo.ed.o cli_installcode.ed.o cli_nvram.ed.o cli_tools.ed.o

$(APP_ZB_DAEMON_ZCZR): $(OBJS_ZB_DAEMON_ZCZR) $(LIBS_ZC)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS) -lreadline -lpthread

$(APP_ZB_DAEMON_ZED): $(OBJS_ZB_DAEMON_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS) -lreadline -lpthread

#-----------  zb_ctl -----------

OBJS_ZB_CTL = zb-ctl.o

$(APP_ZB_CTL): $(OBJS_ZB_CTL) $(LIBS)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS) -lreadline -lpthread

#/* ZBOSS Zigbee software protocol stack
# *
# * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
# * www.dsr-zboss.com
# * www.dsr-corporation.com
# * All rights reserved.
# *
# * Copyright 2024 NXP
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


APP_ZC=thermostat_zc
APP_ZR=thermostat_zr


BINS=$(APP_ZC) $(APP_ZR)


.PHONY: all
all: $(BINS)


.PHONY: clean
clean:
	@rm -f *.o *.d *.map $(BINS)


.PHONY: deploy help
# => refer to $(ROOT_DIR)/template/Options


#----------  onoff_server ----------

ifeq ($(SEPARATE_ZC_ZR),y)
OBJS_ZC = thermostat_zc.zc.o
OBJS_ZR = thermostat_zr.zr.o
else
OBJS_ZC = thermostat_zc.o
OBJS_ZR = thermostat_zr.o
endif


$(APP_ZC): $(OBJS_ZC) $(LIBS_ZC)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP_ZR): $(OBJS_ZR) $(LIBS_ZR)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)



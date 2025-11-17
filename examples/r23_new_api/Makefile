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


APP_ZC=r23_zc
APP_ZR23=r23_zr
APP_ZR22=r22_zr
APP_ZED=r23_zed


BINS=$(APP_ZC) $(APP_ZR22) $(APP_ZR23) $(APP_ZED)


.PHONY: all
all: $(BINS)


.PHONY: clean
clean:
	@rm -f *.o *.d *.map $(BINS)


.PHONY: deploy help
# => refer to $(ROOT_DIR)/template/Options


#----------  r23_new_api ----------

ifeq ($(SEPARATE_ZC_ZR),y)
OBJS_ZC = r23_zc.zc.o
OBJS_ZR23 = r23_zr.zr.o
OBJS_ZR22 = r22_zr.zr.o
else
OBJS_ZC = r23_zc.o
OBJS_ZR23 = r23_zr.o
OBJS_ZR22 = r22_zr.o
endif
OBJS_ZED = r23_zed.ed.o

$(APP_ZC): $(OBJS_ZC) $(LIBS_ZC)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP_ZR23): $(OBJS_ZR23) $(LIBS_ZR)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP_ZR22): $(OBJS_ZR22) $(LIBS_ZR)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP_ZED): $(OBJS_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

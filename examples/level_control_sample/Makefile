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


APP_ZC=level_control_zc
APP1_ZED=level_control_zed_auto_mode
APP2_ZED=level_control_zed_compatibility_mode
APP3_ZED=level_control_zed_legacy_mode


BINS=$(APP_ZC) $(APP1_ZED) $(APP2_ZED) $(APP3_ZED)


.PHONY: all
all: $(BINS)


.PHONY: clean
clean:
	@rm -f *.o *.d *.map $(BINS)


.PHONY: deploy help
# => refer to $(ROOT_DIR)/template/Options


#----------  level_control_sample  ----------

ifeq ($(SEPARATE_ZC_ZR),y)
OBJS_ZC = level_control_zc.zc.o
else
OBJS_ZC = level_control_zc.o
endif
OBJS1_ZED = level_control_zed_auto_mode.ed.o
OBJS2_ZED = level_control_zed_compatibility_mode.ed.o
OBJS3_ZED = level_control_zed_legacy_mode.ed.o


$(APP_ZC): $(OBJS_ZC) $(LIBS_ZC)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP1_ZED): $(OBJS1_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP2_ZED): $(OBJS2_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)
	
$(APP3_ZED): $(OBJS3_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)



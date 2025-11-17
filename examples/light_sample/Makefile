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


APP_ZC=light_coordinator/light_zc
APP_ZR=dimmable_light/bulb
APP_ZED=light_control/light_control


BINS=$(APP_ZC) $(APP_ZR) $(APP_ZED)


.PHONY: all
all: $(BINS)


.PHONY: clean
clean:
	@rm -f */*.o */*.d */*.map $(BINS)


.PHONY: deploy help
# => refer to $(ROOT_DIR)/template/Options


#----------  light_sample ----------

ifeq ($(SEPARATE_ZC_ZR),y)
OBJS_ZC = light_coordinator/light_zc.zc.o light_coordinator/light_zc_hal.zc.o
OBJS_ZR = dimmable_light/bulb.zr.o dimmable_light/bulb_hal.zr.o
else
OBJS_ZC = light_coordinator/light_zc.o light_coordinator/light_zc_hal.o
OBJS_ZR = dimmable_light/bulb.o dimmable_light/bulb_hal.o
endif
OBJS_ZED = light_control/light_control.ed.o light_control/light_control_hal.ed.o


$(APP_ZC): $(OBJS_ZC) $(LIBS_ZC)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP_ZR): $(OBJS_ZR) $(LIBS_ZR)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)

$(APP_ZED): $(OBJS_ZED) $(LIBS_ZED)
	$(CC) -o $@ -Wl,--start-group $^ -Wl,--end-group $(LFLAGS)



#!/bin/sh
#
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2023-2026 NXP
#
#

echo "ZB_APP_NAME          = $ZB_APP_NAME"
echo "ZBOSS_OTA_SERVER_DIR = $ZBOSS_OTA_SERVER_DIR"
echo "MACSPLIT_CHANNEL     = $MACSPLIT_CHANNEL"
echo "MACSPLIT_IEEE_ADDR   = $MACSPLIT_IEEE_ADDR"
echo "MACSPLIT_TTY         = $MACSPLIT_TTY"
echo "ZB_TRACE_LEVEL       = $ZB_TRACE_LEVEL"
echo "ZB_TRACE_MASK        = $ZB_TRACE_MASK"
echo "ZB_APP_OUT           = $ZB_APP_OUT"
echo "DUMP_TTY             = $DUMP_TTY"

# Clean WorkingDirectory
rm -f /var/local/zboss/${ZB_APP_NAME}.log*
rm -f /var/local/zboss/${ZB_APP_NAME}.console*
rm -f /var/local/zboss/${ZB_APP_NAME}.dump
rm -f /var/local/zboss/${ZB_APP_NAME}.prod
rm -f /var/local/zboss/${ZB_APP_NAME}.trace

# OTA Upgrade Server specific:
if [ -n "${ZBOSS_OTA_SERVER_DIR}" ]; then
	mkdir -p ${ZBOSS_OTA_SERVER_DIR}
fi

# X86 bridge specific:
if [ "${ZB_APP_NAME}" = "acm_virtualtty_bridge" ]; then
	res=`lsmod | grep -c g_serial`
	if [ ${res} -eq 0 ]; then
		echo "load g_serial"
		modprobe g_serial n_ports=1 > /dev/null 2>&1
	fi
fi

echo "Start ZBOSS appli ${ZB_APP_NAME}"
/usr/bin/${ZB_APP_NAME}

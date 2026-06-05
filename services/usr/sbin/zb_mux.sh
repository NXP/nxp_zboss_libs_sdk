#!/bin/sh
#
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2023-2026 NXP
#
#

# Restart otbr-agent service if it is active
if systemctl is-active --quiet otbr-agent; then
    echo "Restart otbr-agent service"
    systemctl restart otbr-agent
fi

# Check if btnxpuart has been loaded & started (can blacklisted in /etc//modprobe.d/blacklist.conf)
res=`lsmod | grep -c "btnxpuart"`
if [ ${res} -eq 0 ]; then
	echo "Load btnxpuart"
	modprobe btnxpuart
	sleep 3
fi

# Clean WorkingDirectory
rm -f /var/local/zboss/zb_mux.log*
rm -f /var/local/zboss/zb_mux.console*

export ZB_MUX_OPTIONS="-i /dev/${SPI_dev} -o 0:/tmp/ttyOpenThread -o 2:/tmp/ttyZigbee -s -S ${spi_speed} -m 0 -I ${INT_gpiochip_line}:/dev/${INT_gpiochip_num} -R ${RST_gpiochip_line}:/dev/${RST_gpiochip_num} -M ${reset_15_4_mode}:${reset_15_4_threshold} -t ${mux_trace}"
echo "Starting /usr/sbin/zb_mux ${ZB_MUX_OPTIONS}"
/usr/sbin/zb_mux ${ZB_MUX_OPTIONS}

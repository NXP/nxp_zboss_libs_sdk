#!/bin/bash
#
#
# Copyright 2025 NXP
#
# NXP Proprietary.
# This software is owned or controlled by NXP and may only be used strictly
# in accordance with the applicable license terms. By expressly accepting
# such terms or by downloading, installing, activating and/or otherwise using
# the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to be
# bound by the applicable license terms, then you may not retain, install,
# activate or otherwise use the software.
#
#

SCRIPT_NAME=$0
DEBUG=1
function ECHO()
{
    if [ $DEBUG -eq 1 ]; then
        echo "$SCRIPT_NAME: $1"
    fi
    logger "$SCRIPT_NAME: $1"
}

function do_independant_reset()
{
    ECHO "From device-tree settings -> Retrieve IW61x_OT_RST  gpiochip line"

    RST_gpiochip_num=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $1}'`
    RST_gpiochip_line=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $2}'`

    ECHO "RST:          ${RST_gpiochip_num}:${RST_gpiochip_line}"
    ECHO "Reset CPU2..."
    gpioset -z -c ${RST_gpiochip_num} ${RST_gpiochip_line}=0
    sleep 0.5
    killall -9 gpioset
    gpioset -z -c ${RST_gpiochip_num} ${RST_gpiochip_line}=1
    sleep 0.5
    killall -9 gpioset
}

if [[ $# -ne 2 ]] ; then
    ECHO "Error: Missing arguments: you need to specify Zigbee and Openthread applications to kill !!!"
    exit 1
fi

ECHO "killall ot_zb_dualpan-monitor.sh"
killall -9 ot_zb_dualpan-monitor.sh
sleep 1
ECHO "killall imx-dualpan-failsafe.sh"
killall -9 imx-dualpan-failsafe.sh
sleep 1
ECHO "killall imx-dualpan.sh"
killall -9 imx-dualpan.sh
sleep 1
ECHO "killall $1"
killall -9 $1
sleep 1
ECHO "killall $2"
killall -9 $2
sleep 1
ECHO "killall zb_mux"
killall -9 zb_mux
sleep 1

rmmod btnxpuart &
sleep 1
killall -9 rmmod
sleep 1

do_independant_reset

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

# Parameters
zb_app=""
ot_app=""

function ECHO()
{
    if [ $DEBUG -eq 1 ]; then
        echo "$SCRIPT_NAME: $1"
    fi
    logger "$SCRIPT_NAME: $1"
}

function do_BT_independent_reset()
{
    ECHO "!!! Bluetooth independent_reset.. !!!"
    hcitool -i hci0 cmd 3f fc 00
}

function do_independent_reset()
{
    ECHO "do_independent_reset"

    RST_gpiochip_num=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $1}'`
    RST_gpiochip_line=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $2}'`

    ECHO "RST:          ${RST_gpiochip_num}:${RST_gpiochip_line}"
    ECHO "!!! IW612 Independent Reset !!!"
    gpioset -z -c ${RST_gpiochip_num} ${RST_gpiochip_line}=0
    sleep 0.5
    killall -9 gpioset
    gpioset -z -c ${RST_gpiochip_num} ${RST_gpiochip_line}=1
    sleep 0.5
    killall -9 gpioset
}

function do_hardware_reset()
{
    #ECHO "From device-tree settings -> Retrieve IWxxx_PD_N gpiochip line"

    PD_N_gpiochip_num=`gpioinfo IWxxx_PD_N 2>/dev/null | awk -F" " '{print $1}'`
    PD_N_gpiochip_line=`gpioinfo IWxxx_PD_N 2>/dev/null | awk -F" " '{print $2}'`
    ECHO "PD_N:          ${PD_N_gpiochip_num}:${PD_N_gpiochip_line}"
    ECHO "!!! IW612 Power Down Reset !!!"
    gpioset -z -c ${PD_N_gpiochip_num} ${PD_N_gpiochip_line}=0
    sleep 0.5
    killall -9 gpioset
    gpioset -z -c ${PD_N_gpiochip_num} ${PD_N_gpiochip_line}=1
    sleep 0.5
    killall -9 gpioset
}

function get_bt_hci0_state()
{
    hci_state=$(hciconfig hci0)
    if [[ ${hci_state} == *"DOWN"* ]]; then
      ECHO "hci0 is DOWN"
      touch /tmp/hci0_is_down
    fi
    if [[ ${hci_state} == *"UP"* ]]; then
      ECHO "hci0 is UP"
    fi
}
function kill_previous_run()
{
        killall -9 -q $zb_app
        killall -9 -q $ot_app
        sleep 1
        killall -9 -q zb_mux
        killall btmon
}

function monitor_app()
{
    ot_app_pid=$(pidof $1)
    while kill -0 "$ot_app_pid"
    do
        #ECHO "$1 still alive..."
        sleep 0.5
    done
    ECHO "$1 EXITED -> Creating $2 !"
    touch $2
}

function waiting_for_app_to_start()
{
    pidof $1
    while [ $? -ne 0 ]
    do
        #ECHO "Waiting for $1 to start ..."
        sleep 0.5
        pidof $1
    done
    ECHO "$1 STARTED -> Creating $2 !"
    touch $2
}

function exit_properly()
{
    killall -6 imx-dualpan.sh

    sleep 0.5
    kill_previous_run
    zb_mux_pid=$(pidof zb_mux)
    while kill -0 "$zb_mux_pid"
    do
        ECHO "zb_mux still running..."
        sleep 0.5
    done
    sleep 0.5
    do_BT_independent_reset
    sleep 1
    ECHO "rmmod btnxpuart &.."
    rmmod btnxpuart &
    res=$(lsmod | grep -c btnxpuart)
    ECHO "!!! Waiting for btnxpuart to be removed... !!!"
    while [ ${res} -eq 1 ]
    do
        ECHO "!!! lsmod | grep -c btnxpuart: ${res} !!!"
        res=$(lsmod | grep -c btnxpuart)
        sleep 0.5
    done
    ECHO "!!! btnxpuart removed properly !!!"
}
function main()
{
    rm -f /tmp/zb_app_started
    rm -f /tmp/ot_app_started
    rm -f /tmp/zb_app_exited
    rm -f /tmp/ot_app_exited
    rm -f /tmp/hci0_is_down
    if [ -z "$1" ]; then
        max_time_to_perform_IR=10
    else
        max_time_to_perform_IR=$1
    fi
    if [ -z "$2" ]; then
        time_unit=S
    else
        time_unit=$2
    fi
    if [ $time_unit == "M" ] ; then
        #ECHO "time_unit: $time_unit"
        max_time_to_perform_IR=$(($max_time_to_perform_IR * 60))
    fi
    #ECHO "Time window duration = $max_time_to_perform_IR seconds"

    if [ -z "$3" ]; then
        echo "ERROR: Zigbee Application must be set as parameter"
        exit -1
    else
        zb_app=$3
        ECHO "Zigbee Application = $zb_app"
    fi
    if [ -z "$4" ]; then
        echo "ERROR: Openthread Application must be set as parameter"
        exit -1
    else
        ot_app=$4
        ECHO "Openthread Application = $ot_app"
    fi

    waiting_for_app_to_start ${zb_app} "/tmp/zb_app_started" &
    waiting_for_app_to_start ${ot_app} "/tmp/ot_app_started" &

    while [ ! -f /tmp/zb_app_started ] || [ ! -f /tmp/ot_app_started ]
    do
        #ECHO "Waiting for $zb_app and $ot_app to start..."
        sleep 0.5
    done

    # Check that Bluetooth hci0 interface is UP
    rm -f /tmp/hci0_is_down
    get_bt_hci0_state
    if [ -f /tmp/hci0_is_down ];then
        # If not UP but DOWN, full reset of IWxxx is required...
        # because in that case, btnxpuart sees "Bluetooth: hci0: FW already running."
        exit_properly
        ECHO "do_hardware_reset"
        sleep 0.5
        do_hardware_reset
    else
        ECHO "!!! MONITORING STARTED: ${ot_app} and ${zb_app} !!!"
        start_time=$(date -u +%s)
        #ECHO "start_time= $start_time"

        monitor_app ${zb_app} "/tmp/zb_app_exited" &
        monitor_app ${ot_app} "/tmp/ot_app_exited" &

        end_time=$(date -u +%s)
        #ECHO "end_time= $end_time"

        time_diff=$(expr $end_time - $start_time)
        #ECHO "time_diff=$time_diff"

        while [ ! -f /tmp/zb_app_exited ] && [ ! -f /tmp/ot_app_exited ]
        do
            #ECHO "Both $zb_app and $ot_app are still running..."
            sleep 0.5
        done
        sleep 5

        ECHO "${ot_app} or ${zb_app} exited, kill imx-dualpan-real.sh and exit properly"
        exit_properly
        sleep 0.5
        #do_independent_reset
    fi

    rm -f /tmp/zb_app_started
    rm -f /tmp/ot_app_started
    rm -f /tmp/zb_app_exited
    rm -f /tmp/ot_app_exited

    touch /tmp/restart_imx_dualpan
}

if [[ $# -ne 4 ]] ; then
    ECHO "Error: Missing arguments"
    ECHO "Usage: ot-host-monitor.sh [time value: integer > 0]  [time unit: M or S] Zigbee_Application Openthread_Application "
    ECHO "Example: ot-host-monitor.sh 1 M nxp_cli ot-daemon"
    exit 1
else
    if [[ $1 -lt 1 ]] ; then
        ECHO "Error: Wrong first argument"
        ECHO '-> time value must be greater than 0'
        exit 1
    fi
    if [[ ($2 != "M") && ($2 != "S") ]] ; then
        ECHO "Error: Wrong second argument"
        ECHO '-> time unit must be M(minutes) or S(seconds)'
        exit 1
    fi
    if [[ ($2 == "S") && ($1 -lt 60) ]] ; then
        ECHO "Error: Wrong time window duration $1 $2"
        ECHO '-> choose a time window duration greater than or equal to 60 seconds'
        exit 1
    fi
fi

if ps -aux | grep 'imx-dualpan.sh' | grep '\--ot' | grep -q '\--zb'; then
    mode="dualPAN"
    ECHO "mode dualPAN detected"
elif ps -aux | grep 'imx-dualpan.sh' | grep -q '\--ot'; then
    mode="singlePAN_UART"
    ECHO "mode singlePAN_UART detected"
else
    ECHO "imx-dualpan.sh is not running, assume ${ot_app} is running through SPI"
    ECHO "mode singlePAN_SPI"
    mode="singlePAN_SPI"
fi

main "$@"


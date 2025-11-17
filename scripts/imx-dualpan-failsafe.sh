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

channel=""
zb_app=""
ot_app=""
# Skip Script Interaction Questions
skip_questions=0

function validate_firmware()
{
    if [ -z "${firmware}" ]; then
        # Firmware is optional, do not fail
        echo "No Firmware configured"; return
    fi
    if [ ! -f ${firmware} ]; then
        echo "Firmware file ${firmware} does not exist"; echo "usage: ${me} --help"; exit 1
    fi
}
function validate_channel()
{
    if [ ${channel} -lt 11 ] || [ ${channel} -gt 26 ]; then
        echo "Invalid zigbee channel ${channel}"; echo "usage: ${me} --help"; exit 1
    fi
}
function validate_zb_app()
{
    if [ -z "${zb_app}" ]; then
        echo "No Zigbee Application configured"; echo "usage: ${me} --help"; exit 1
    fi
    if [ ! -f ${zb_app} ]; then
        echo "Application file ${zb_app} does not exist"; echo "usage: ${me} --help"; exit 1
    fi
    if [ -z "${first}" ]; then
        first=zb_app
    else
        second=zb_app
    fi
}
function validate_ot_app()
{
    if [ -z "${ot_app}" ]; then
        echo "No Openthread Application configured"; echo "usage: ${me} --help"; exit 1
    fi
    if [ ! -f ${ot_app} ]; then
        echo "Application file ${ot_app} does not exist"; echo "usage: ${me} --help"; exit 1
    fi
    if [ -z "${first}" ]; then
        first=ot_app
    else
        zb_cli=`echo ${zb_app} | grep -c "cli_nxp"`
        if [ ${zb_cli} -gt 0 ]; then
            echo "Application Zigbee ${zb_app} is a CLI, it cannot be run first in dualpan (would be in background)"; echo "usage: ${me} --help"; exit 1
        else
            second=ot_app
        fi
    fi

    # otbr-agent uses DBus, not ot-daemon
    res=`objdump -x ${ot_app} | grep -c "DBusMessageExtract\|DBusMessageEncode"`
    if [ ${res} -gt 0 ]; then
        otbr=yes
    else
        otbr=no
    fi
}
function validate_ot_cfg()
{
    case ${ot_cfg} in
        # FTD:
        router|Router|ROUTER) ot_cfg=router ;;
        reed|Reed|REED)       ot_cfg=reed   ;;
        fed|Fed|FED)          ot_cfg=fed    ;;
        # MTD:
#        med|Med|MED)          ot_cfg=med    ;;
#        sed|Sed|SED)          ot_cfg=sed    ;;
        # Error:
        *)  echo "Unknown Thread config: ${ot_cfg}"; echo "usage: ${me} --help"; exit 1
    esac
}
function validate_ot_net()
{
    case ${ot_net} in
        create|Create|CREATE) ot_net=create ;;
        join|Join|JOIN)       ot_net=join    ;;
        *)  echo "Unknown Thread Network: ${ot_net}"; echo "usage: ${me} --help"; exit 1
    esac
}
function validate_ot_region()
{
    if [[ ! ${ot_region} =~ ^[A-Z] ]]; then
        echo "Invalid otregion ${ot_region}, must be in CAPITAL letters"; echo "usage: ${me} --help"; exit 1
    fi
}

function validate_imx8_reworked()
{
    soc_id=`cat /sys/devices/soc0/soc_id`
    if [ "${soc_id}" != "i.MX8MM" ]; then
        echo "WARNING: option --rw not available for ${soc_id}, it is for i.MX8MM with SPI_INT corrected to R1003"
        imx8_reworked=0
    fi
}

MINIMUM_KERNEL_VERSION=6
MINIMUM_KERNEL_PATCHLEVEL=12
MINIMUM_KERNEL_SUBLEVEL=3
function check_bsp_version()
{
    kernel_ver=$(echo $1 | cut -d '.' -f1);patchlevel_ver=$(echo $1 | cut -d '.' -f2);sublevel_ver=$(echo $1 | cut -d '.' -f3)
    if [ ${kernel_ver} -lt ${MINIMUM_KERNEL_VERSION} ];then return 0;fi
    if [ ${kernel_ver} -gt ${MINIMUM_KERNEL_VERSION} ];then return 1;fi
    if [ ${patchlevel_ver} -lt ${MINIMUM_KERNEL_PATCHLEVEL} ];then return 0;fi
    if [ ${patchlevel_ver} -gt ${MINIMUM_KERNEL_PATCHLEVEL} ];then return 1;fi
    if [ ${sublevel_ver} -ge ${MINIMUM_KERNEL_SUBLEVEL} ];then return 1;fi
}

function main()
{
    bsp_ver=$(uname -r | cut -d '-' -f1)
    check_bsp_version ${bsp_ver};bsp_ver_ret=$?
    if [ ${bsp_ver_ret} -eq 0 ];then
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "! BAD BSP VERSION (uname -r = ${bsp_ver} ) -> ${MINIMUM_KERNEL_VERSION}.${MINIMUM_KERNEL_PATCHLEVEL}.${MINIMUM_KERNEL_SUBLEVEL} or higher is mandatory !"
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        exit 1
    fi
    rm -f /tmp/restart_imx_dualpan
    rm -f *imx-dualpan-recovery.tar*
    rm -f hci_dumps.pcap

    # Save imx-dualpan.sh parameters
    params="$@"

    # Force --sk option
    params="$params --sk"
    ECHO "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    ECHO "! Skip Questions Option Activated: no need to interact with imx-dualpan.sh questions !"
    ECHO "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    ECHO "Parameters: $params"

    while [ "$#" -gt 0 ]; do
        case $1 in
        --help)      usage; exit 0;;
        # mandatory
        --ch)        shift; channel=$1;   validate_channel  ;;
        # at least one
        --zb)        shift; zb_app=$1;    validate_zb_app   ;;
        --ot)        shift; ot_app=$1;    validate_ot_app   ;;
        # optional
        --fw)        shift; firmware=$1;  validate_firmware ;;
        --spi)       shift; spi_speed=$1  ;;
        --csdelay)   shift; spi_cs_delay=$1 ;;
        --ieee)      shift; ieee_addr=$1  ;;
        --otcfg)     shift; ot_cfg=$1;    validate_ot_cfg   ;;
        --otnet)     shift; ot_net=$1;    validate_ot_net   ;;
        --otprefix)  shift; ot_prefix=$1  ;;
        --otregion)  shift; ot_region=$1; validate_ot_region ;;
        --brbb)      shift; otbr_backbone=$1 ;;
        --ping)      shift; otbr_ping=$1 ;;
        # debug
        --zberr)     mux_track_log_error=1; app_track_log_error=1 ;;
        --zblog)     shift; run_level=$1                          ;;
        --otlog)     shift; thread_log="$1 -v"                    ;;
        --muxout)    shift; export ZB_MUX_OUT=$1                  ;;
        --zbout)     shift; export ZB_APP_OUT=$1                  ;;
        --dbgspi)    shift; export DUMP_SPI=$1                    ;;
        --dbgspinel) shift; export DUMP_SPINEL=$1                 ;;
        --dbgtty)    shift; export DUMP_TTY=$1                    ;;
        --gdbmux)    shift; gdb_mux=$1                            ;;
        --gdbapp)    shift; gdb_app=$1                            ;;
        --core)      coredump=1                                   ;;
        --zbclean)   zbclean=1                                    ;;
        --logsh)     shift; export ZB_LOGFILE_POST_ROTATE_SH=$1   ;;
        --sk)        shift; skip_questions=1                      ;;
        --rw)        imx8_reworked=1; validate_imx8_reworked      ;;
        *)           echo "Unknown option $1"; echo "usage: ${me} --help"; exit 1 ;;
        esac
        shift
    done

    while true
    do
        rm -f /tmp/restart_imx_dualpan
        zb_app_temp="${zb_app%/}";zb_app="${zb_app_temp##*/}"
        ot_app_temp="${ot_app%/}";ot_app="${ot_app_temp##*/}"

        # Start ZIGBEE and OPENTHREAD APPLICATIONS MONITORING in background
        ./ot_zb_dualpan-monitor.sh 1 M ${zb_app} ${ot_app} &

        # call imx-dualpan.sh with params
        ECHO "!!! Calling ./imx-dualpan.sh ${params} !!!"

        ./imx-dualpan.sh ${params}
        #kill $pidof_monitor

        while [ ! -f /tmp/restart_imx_dualpan ]
        do
            ECHO "!!! $0 back again -> wait for Independent Reset performed !!!"
            sleep 1
        done
        killall -9 ot_zb_dualpan-monitor.sh

        # save log files
        date=$(date -u |tr -d ' '|tr -d ':')
        tarname="${date}-imx-dualpan-recovery.tar"
        tar -cvf ${tarname} --absolute-names  /var/log/syslog ./zb_mux.log ./zb_mux.console* ./${zb_app}.log ./${zb_app}.console* ./${zb_app}.nvram ./hci_dumps.pcap
        rm ./zb_mux.log ./zb_mux.console* ./${zb_app}.log ./${zb_app}.console* hci_dumps.pcap

        ECHO "!!! RECOVERY IN PROGRESS for imx-dualpan.sh !!!"
    done

}

main "$@"

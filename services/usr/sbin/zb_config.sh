#!/bin/sh
#
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2023-2026 NXP
#
#

ZB_ENV_VARS_FILE="/tmp/zb_environment_vars"

function create_env_file() {
    echo "#WARNING, this file was created by '\"$(basename $0)"'/' > ${ZB_ENV_VARS_FILE}
    cat << EOF >> ${ZB_ENV_VARS_FILE}
SPI_dev=$SPI_dev
RST_gpiochip_num=$RST_gpiochip_num
RST_gpiochip_line=$RST_gpiochip_line
INT_gpiochip_num=$INT_gpiochip_num
INT_gpiochip_line=$INT_gpiochip_line
spi_speed="${spi_speed}"
reset_15_4_mode="${reset_15_4_mode}"
reset_15_4_threshold="${reset_15_4_threshold}"
mux_trace="${mux_trace}"
ZB_MUX_OUT="${ZB_MUX_OUT}"
DUMP_SPI="${DUMP_SPINEL}"
ZB_APP_NAME="${ZB_APP_NAME}"
ZBOSS_OTA_SERVER_DIR="${ZBOSS_OTA_SERVER_DIR}"
MACSPLIT_CHANNEL="${MACSPLIT_CHANNEL}"
MACSPLIT_IEEE_ADDR="${MACSPLIT_IEEE_ADDR}"
MACSPLIT_TTY="${MACSPLIT_TTY}"
ZB_TRACE_LEVEL="${ZB_TRACE_LEVEL}"
ZB_TRACE_MASK="${ZB_TRACE_MASK}"
ZB_APP_OUT="${ZB_APP_OUT}"
DUMP_TTY="${DUMP_TTY}"
EOF
}

function detect_gpio() {
    gpioinfo IWxxx_NB_IND_RST_15_4
    ret=$?
    if [ $ret != 0 ]; then
        echo "#ERROR your Device Tree does not contain GPIO Naming feature, please update it together with the BSP"
        exit -1
    fi
    RST_gpiochip_num=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $1}'`
    RST_gpiochip_line=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $2}'`
    INT_gpiochip_num=`gpioinfo IWxxx_NB_SPI_INT 2>/dev/null | awk -F" " '{print $1}'`
    INT_gpiochip_line=`gpioinfo IWxxx_NB_SPI_INT 2>/dev/null | awk -F" " '{print $2}'`

    spi0=`find /sys/firmware/devicetree/base -name spi@0`
    if [ -f $spi0/label ] && [ `tr -d '\0' < $spi0/label` = "IWxxx_SPIDEV" ]; then
        num=$(echo $spi0 | awk -F/ '{print NF}');let num=num-1
        spi_bus_addr=$(echo $spi0 | cut -d '/' -f$num | cut -d "@" -f2)
        spi_folder=$(find /sys/devices/platform -type d -name $spi_bus_addr.spi*)
        SPI_dev=$(find $spi_folder -type d -name spidev*.* | awk -F/ '{print $NF}')
    fi

    export SPI_dev=$SPI_dev
    export RST_gpiochip_num=$RST_gpiochip_num
    export RST_gpiochip_line=$RST_gpiochip_line
    export INT_gpiochip_num=$INT_gpiochip_num
    export INT_gpiochip_line=$INT_gpiochip_line
}

function start() {
    # Create WorkingDirectory
    # Caution: cannot get rid of this since it must exist before running zb_mux/zb_app
    mkdir -p /var/local/zboss
    detect_gpio
    create_env_file
}

function stop() {
    if [ -e ${ZB_ENV_VARS_FILE} ]; then
        rm -rf ${ZB_ENV_VARS_FILE}
    fi
}

case "$1" in
start)
    start
    ;;
stop)
    stop
    ;;
*)
    echo "usage: $0 start|stop"
esac

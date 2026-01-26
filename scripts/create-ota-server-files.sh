#!/bin/bash
#
#
# Copyright 2024-2025 NXP
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

#//bin/sh

me=$0

# Zboss host (stack 42.20, build 19.2503.001)
zboss_host_tag=019.2601.028
zboss_stack=zoi_release-4.2.2.0

# Firmware (stack 18.99, build 3p23.0)
iw612_version=18.99.3p23.0

# Please update the following according to your configuration

zb_folder=~/zboss-dual-pan
zb_services_folder=${zb_folder}/services
zb_services_files="zb_config.service zb_config.sh zb_mux.service zb_mux.env zb_app.service zb_app.env"
zb_apps_folder=${zb_folder}/prebuild
#zb_apps_files="bulb light_control light_zc on_off_output_zc on_off_switch_zed ota_simple_download_zc ota_simple_download_zr simple_gw thermostat_zc thermostat_zr"
zb_apps_files="on_off_output_zc on_off_switch_zed simple_gw"


fw_folder=~/iw612-firmware
fw_files="uartspi_n61x_v1-${iw612_version}.bin uartspi_n61x_v1-${iw612_version}.prod.bin.se"


function get_zb_version()
{
	local stack=$1
	local host_tag=$2

	# zboss_host_tag is <certif-tag>.<year<<2|quarter>.<index>, ignore certif-tag
	local appli_year=`echo ${host_tag} | awk -F "." '{print $2}' | awk -F "0" '{print $1}'`
	local appli_quarter=`echo ${host_tag} | awk -F "." '{print $2}' | awk -F "0" '{print $2}'`
	local appli_index=`echo ${host_tag} | awk -F "." '{print $3}'`
	if [ ${appli_index} -gt 255 ]; then
		appli_index=0
	fi

	# Pack to 6 bits year & 2 bits quarter a convert to hex
	appli_yearQ=$(((appli_year << 2) + (appli_quarter - 1)))
	local appli_rel=$(printf "%02x" ${appli_yearQ} )
	local appli_bld=$(printf "%02x" ${appli_index})

	# zboss_stack is zoi_release-4.2.2.0 & up
	local stack_rel=`echo ${stack} | awk -F "-" '{print $2}' | awk -F "." '{print $1$2}'`
	local stack_bld=`echo ${stack} | awk -F "-" '{print $2}' | awk -F "." '{print $3$4}'`
	
	echo "${appli_rel}${appli_bld}${stack_rel}${stack_bld}"
}

function get_fw_version()
{
	local version=$1

	# iw612_version is <stack_rel>.<stack_bld>.<appli_rel>p<appli_bld>.x
	local stack_rel=`echo ${version} | awk -F "." '{print $1}'`
	local stack_bld=`echo ${version} | awk -F "." '{print $2}'`
	local appli_rel=`echo ${version} | awk -F "." '{print $3}' | awk -F "p" '{print $1}'`
	if [ ${#appli_rel} -lt 2 ]; then appli_rel="0${appli_rel}"; fi # if string len is less than 2 add "0"
	local appli_bld=`echo ${version} | awk -F "." '{print $3}' | awk -F "p" '{print $2}'`
	if [ ${#appli_bld} -lt 2 ]; then appli_bld="0${appli_bld}"; fi # if string len is less than 2 add "0"
	
	echo "${appli_rel}${appli_bld}${stack_rel}${stack_bld}"
}


function check_inputs()
{
	this_file=$1
	this_version=$2
	if [ ! -f  ${this_file} ]; then
		echo "${this_file} is not a file"
		exit 0
	fi

	if [ -z "${this_version}" ]; then
		echo "no version provided"
		exit 0
	fi
}

function create_ota_file()
{
	this_file=$1
	this_version=$2

	check_inputs ${this_file} ${this_version}

	case ${this_file} in
		# Host
		#
		*imx-dualpan.sh)            to_strip=0 ; ota_file=1037-1070-${this_version}-imx-dualpan.sh.zigbee            ;;
		*imx-wifi.sh)               to_strip=0 ; ota_file=1037-1071-${this_version}-imx-wifi.sh.zigbee               ;;
		*x86-zigbee.sh)             to_strip=0 ; ota_file=1037-1075-${this_version}-x86-zigbee.sh.zigbee             ;;
		*zb_config.service)         to_strip=0 ; ota_file=1037-1080-${this_version}-zb_config.service.zigbee         ;;
		*zb_config.sh)              to_strip=0 ; ota_file=1037-1081-${this_version}-zb_config.sh.zigbee              ;;
		*zb_mux.service)            to_strip=0 ; ota_file=1037-1083-${this_version}-zb_mux.service.zigbee            ;;
		*zb_mux.sh)                 to_strip=0 ; ota_file=1037-1084-${this_version}-zb_mux.env.zigbee                ;;
		*zb_mux.env)                to_strip=0 ; ota_file=1037-1085-${this_version}-zb_mux.env.zigbee                ;;
		*zb_app.service)            to_strip=0 ; ota_file=1037-1086-${this_version}-zb_app.service.zigbee            ;;
		*zb_app.sh)                 to_strip=0 ; ota_file=1037-1087-${this_version}-zb_app.env.zigbee                ;;
		*zb_app.env)                to_strip=0 ; ota_file=1037-1088-${this_version}-zb_app.env.zigbee                ;;
		*zb_mux)                    to_strip=1 ; ota_file=1037-1100-${this_version}-zb_mux.zigbee                    ;;
		*simple_gw)                 to_strip=1 ; ota_file=1037-1400-${this_version}-simple_gw.zigbee                 ;;
		*on_off_output_zc)          to_strip=1 ; ota_file=1037-1401-${this_version}-on_off_output_zc.zigbee          ;;
		*on_off_switch_zed)         to_strip=1 ; ota_file=1037-1402-${this_version}-on_off_switch_zed.zigbee         ;;
		*light_zc)                  to_strip=1 ; ota_file=1037-1403-${this_version}-light_zc.zigbee                  ;;
		*bulb)                      to_strip=1 ; ota_file=1037-1404-${this_version}-bulb.zigbee                      ;;
		*light_control)             to_strip=1 ; ota_file=1037-1405-${this_version}-light_control.zigbee             ;;
		*on_off_distrib_output_zr)  to_strip=1 ; ota_file=1037-1406-${this_version}-on_off_distrib_output_zr.zigbee  ;;
		*on_off_distrib_switch_zed) to_strip=1 ; ota_file=1037-1407-${this_version}-on_off_distrib_switch_zed.zigbee ;;
		*thermostat_zc)             to_strip=1 ; ota_file=1037-1408-${this_version}-thermostat_zc.zigbee             ;;
		*thermostat_zr)             to_strip=1 ; ota_file=1037-1409-${this_version}-thermostat_zr.zigbee             ;;
		*ota_simple_download_zc)    to_strip=1 ; ota_file=1037-140A-${this_version}-ota_simple_download_zc.zigbee    ;;
		*ota_simple_download_zr)    to_strip=1 ; ota_file=1037-140B-${this_version}-ota_simple_download_zr.zigbee    ;;
		*ota_server_zc)             to_strip=1 ; ota_file=1037-140C-${this_version}-ota_server_zc.zigbee             ;;
		*ota_client_zr)             to_strip=1 ; ota_file=1037-140D-${this_version}-ota_client_zr.zigbee             ;;
		# NEXT apps come here...
		#
		# Firmware
		#
		*sd_w61x*.prod.bin.se)       to_strip=0 ; ota_file=1037-6120-${this_version}-sd_w61x.prod.bin.se.zigbee       ;;
		*sd_w61x*.eng.bin.se)        to_strip=0 ; ota_file=1037-6120-${this_version}-sd_w61x.eng.bin.se.zigbee        ;;
		*sd_w61x*.bin)               to_strip=0 ; ota_file=1037-6122-${this_version}-sd_w61x.bin.zigbee               ;;
		*sduart_nw61x*.prod.bin.se)  to_strip=0 ; ota_file=1037-6124-${this_version}-sduart_nw61x.prod.bin.se.zigbee  ;;
		*sduart_nw61x*.eng.bin.se)   to_strip=0 ; ota_file=1037-6125-${this_version}-sduart_nw61x.eng.bin.se.zigbee   ;;
		*sduart_nw61x*.bin)          to_strip=0 ; ota_file=1037-6126-${this_version}-sduart_nw61x.bin.zigbee          ;;
		*uartuart_n61x*.prod.bin.se) to_strip=0 ; ota_file=1037-6128-${this_version}-uartuart_n61x.prod.bin.se.zigbee ;;
		*uartuart_n61x*.eng.bin.se)  to_strip=0 ; ota_file=1037-6129-${this_version}-uartuart_n61x.eng.bin.se.zigbee  ;;
		*uartuart_n61x*.bin)         to_strip=0 ; ota_file=1037-612A-${this_version}-uartuart_n61x.bin.zigbee         ;;
		*uartspi_n61x*.prod.bin.se)  to_strip=0 ; ota_file=1037-612C-${this_version}-uartspi_n61x.prod.bin.se.zigbee  ;;
		*uartspi_n61x*.eng.bin.se)   to_strip=0 ; ota_file=1037-612D-${this_version}-uartspi_n61x.eng.bin.se.zigbee   ;;
		*uartspi_n61x*.bin)          to_strip=0 ; ota_file=1037-612E-${this_version}-uartspi_n61x.bin.zigbee          ;;
		#
		# Error
		#
		*)                          echo "unknown file ${this_file}" ; return ;;
	esac
	
	mkdir -p ota-server-files
	cp  ${this_file} ota-server-files/${ota_file}
	if [ ${to_strip} -eq 1 ]; then
		echo "copy & strip ${this_file} in ota-server-files/${ota_file}"
		strip ota-server-files/${ota_file}
	else
		echo "copy ${this_file} in ota-server-files/${ota_file}"
	fi
}


zb_version=$(get_zb_version ${zboss_stack} ${zboss_host_tag})
echo "zb_version: ${zb_version} (stack: ${zboss_stack}, appli: ${zboss_host_tag})"

fw_version=$(get_fw_version ${iw612_version})
echo "fw_version: ${fw_version} (iw612: ${iw612_version})"

# Create common mux & script
if [ -f ${zb_folder}/zb_mux ]; then
	create_ota_file ${zb_folder}/zb_mux          ${zb_version}
fi
if [ -f ${zb_folder}/imx-dualpan.sh ]; then
	create_ota_file ${zb_folder}/imx-dualpan.sh  ${zb_version}
fi
if [ -f ${zb_folder}/imx-wifi.sh ]; then
	create_ota_file ${zb_folder}/imx-wifi.sh     ${zb_version}
fi
if [ -f ${zb_folder}/scripts/x86-zigbee.sh ]; then
	create_ota_file ${zb_folder}/scripts/x86-zigbee.sh  ${zb_version}
fi

# Create services
if [ -d ${zb_services_folder} ]; then
	for file in ${zb_services_files}; do
		thisone=`find ${zb_services_folder} -name ${file}`
		[ -f ${thisone} ] && create_ota_file ${thisone} ${zb_version} || true
	done
fi

# Create zb_apps
if [ -d ${zb_apps_folder} ]; then
	for file in ${zb_apps_files}; do
		create_ota_file ${zb_apps_folder}/${file} ${zb_version}
	done
fi

# Create firmwares
if [ -d ${fw_folder} ]; then
	for file in ${fw_files}; do
		create_ota_file ${fw_folder}/${file}      ${fw_version}
	done
fi

#!/bin/bash
#
#
# Copyright 2023-2025 NXP
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

gdb_mux=${GDB_MUX}
gdb_app=${GDB_APP}

spi_speed=1000000
spi_cs_delay=0

# RESET 15.4 Configuration (disable/softReset/IR(caution:shut-down also BTBLE))
reset_15_4_mode=0
reset_15_4_threshold=300

#mux_trace=4:0xffffffff
mux_trace=0:0

# runtime zboss log level for applications
run_level=release
#run_level=debug

# runtime thread log level
thread_log=4

# track error message from zbosss log file: 0: no track, 1: do track
mux_track_log_error=0
app_track_log_error=0

# generate coredump in ./coredump in case of crash of the app
coredump=0

# cleanup all previous execution of zigbee (removes *.log, *.nvram, *.prod, *.dump, *.trace files)
zbclean=0

# Skip Script Interaction Questions
skip_questions=0

function usage()
{
	command="${me} [options]"
	echo
	echo "${command}"
	echo
	echo "Mandatory:"
	echo "  --ch <channel>             OpenThread & Zigbee channel to use (but not for cli_nxp or zb_daemon)"
	echo
	echo "Options:"
	echo "  --ot <ot appli>            Openthread application (uart-hdlc) (border router is auto-detected)"
	echo "  --zb <zb appli>            Zigbee application"
	echo "  --fw <firmware>            Firmware file to download (optional)"
	echo "  --chip <IW612|IW610>       Define which chipset is used (default IW612)"
	echo "  --sk                       Skip Script Interaction Questions"
	echo
	echo "At least one appli is mandatory, either zigbee or thread"
	echo
	echo "Advanced (optional):"
	echo "  --help                     This help"
	echo "  --spi <speed/Hz>           Configure spi_speed"
	echo "  --csdelay <CS delay/µs>    Configure spi_chip_select_setup_delay"
	echo "  --ieee <ieee_addr>         Configure ieee address (zigbee but not for cli_nxp or zb_daemon)"
	echo "  --otcfg <router|reed|fed>  Configure thread role (requires --otnet)"
	echo "  --otnet <create|join>      Create or join existing thread (required by --otcfg)"
	echo "  --otprefix <prefix>        Configure ot prefix, default ${ot_prefix} (required by --otnet create on border router)"
	echo "  --otregion <region>        Configure ot region domain mapping value in uppercase"
	echo "  --brbb <ifname>            Configure otbr-agent backbone (default: eth0), used only if \'ot appli\' is detected as otbr-agent"
	echo "  --ping <ipv6_addr>         Ping <ipv6_addr> with 1K payload, requires to configure thread role & having a otbr-agent between <ipv6_addr> and the thread network"
	echo
	echo "The first appli started is the first one provided in the list of arguments"
	echo "Start Thread then Zigbee: ${me} --ch <channel> --ot <thread appli> --zb <zb appli> [--fw <firmware>]"
	echo "Start Zigbee then Thread: ${me} --ch <channel> --zb <zb appli> --ot <thread appli> [--fw <firmware>]"
	echo
	echo "Note: to be able to run a Zigbee application with a console (zb_cli, ...), it needs"
	echo "- to run alone (without Thread application)"
	echo "- to run second (with Thread appliaction started first)"
	echo "Note: same restrictions to be able to restart Zigbee application"
	echo
	echo "Debugging:"
	echo "  --zberr                    Track zb_mux & zb appli errors in zboss log file"
	echo "  --dbgspi <0~3>             zb_mux debug SPI frames, bit field: 0: no log, 1: dump SPI header, 2: dump SPI data, 3: both raw & interpreted"
	echo "  --dbgspinel <0~7>          zb_mux debug SPINEL frames, bit field: 0: no log, 1: dump SPINEL Raw data, 2: dump SPINEL interpreted data, 3: both raw & interpreted, 4: dump last SPINEL on app disconnect (enabled by default)"
	echo "  --otlog <0~6>              Openthread application log level (verbose forced)"
	echo "  --dbgtty <0~3>             zb_app debug TTY frames, bit field: 0: dump TTY Raw data, 2: dump SPINEL in TTY-HDLC interpreted data, 3: both raw & interpreted"
	echo "  --muxout <0~3>             zb_mux output bit field: 0: no log (wcs) on console, 1: log (wcs) on console (default), 2: log (wcs) on file zb_mux.console, 3: both"
	echo "  --zbout <0~3>              zb_app output bit field: 0: no log (wcs) on console, 1: log (wcs) on console (default), 2: log (wcs) on file zb_app.console, 3: both"
	echo "  --zblog <debug|release>    Zigbee Zboss log level (on file ${zb_app}.log)"
	echo "  --gdbmux <port>            gdb debugging zb_mux on port specified"
	echo "  --gdbapp <port>            gdb debugging zb_app on port specified"
	echo "  --core                     generate coredump in ./coredumps in case of crash of the app (useful for debug build only)"
	echo "  --zbclean                  cleanup all previous execution of zigbee (removes *.log, *.nvram, *.prod, *.dump, *.trace files)"
	echo "  --logsh <postlogrotatesh>  script to be called by zb_mux/zb_app after a log file is rotated (every 50MB, keep only 3 last logs)"
	echo "  --rw                       i.MX8 Mini rework for direct SPI_INT"
	echo
	echo "spi_speed:   ${spi_speed}"
	echo "mux_trace:   ${mux_trace}"
	echo "run_level:   ${run_level}"
	echo "thread_log:  ${thread_log}"
	echo "DUMP_SPI:    ${DUMP_SPI}"
	echo "DUMP_SPINEL: ${DUMP_SPINEL}"
	echo "DUMP_TTY:    ${DUMP_TTY}"
	echo "DUMP_HCI:    ${DUMP_HCI}"
	echo
}

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
#		med|Med|MED)          ot_cfg=med    ;;
#		sed|Sed|SED)          ot_cfg=sed    ;;
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

# Parse args
me=$0
channel=""
first=""
second=""
otbr=undefined
otbr_backbone=eth0
otbr_ping=""
ot_net=""
ot_prefix="fd11:22::/64"
ot_region=""

imx8_reworked=0

# for zb_mux
# Bit field: 0: no log (wcs) on console, 1: log (wcs) on console, 2: log (wcs) on file zb_mux.console, 3: both
export ZB_MUX_OUT=1
# Bit field: 0: no log, 1: dump SPI header, 2: dump SPI data, 3: both
export DUMP_SPI=0
# Bit field: 0: no log, 1: dump SPINEL Raw data, 2: dump SPINEL interpreted data, 3: both, 4: dump last SPINEL on disconnect
export DUMP_SPINEL=4

# for ${zb_app}
# Bit field: 0: no log (wcs) on console, 1: log (wcs) on console, 2: log (wcs) on file $(zb_app).console, 3: both
export ZB_APP_OUT=1
# Bit field: 0: no log, 1: dump TTY Raw data, 2: dump SPINEL in TTY-HDLC interpreted data, 3: both
export DUMP_TTY=0

# Bit field: 0: no log, 1: dump HCI Raw data
export DUMP_HCI=1

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
	--chip)      shift; chip_name=$1  ;;
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
	--dbghci)    shift; export DUMP_HCI=$1                    ;;
	--gdbmux)    shift; gdb_mux=$1                            ;;
	--gdbapp)    shift; gdb_app=$1                            ;;
	--core)      coredump=1                                   ;;
	--zbclean)   zbclean=1                                    ;;
	--logsh)     shift; export ZB_LOGFILE_POST_ROTATE_SH=$1   ;;
    --sk)        shift; skip_questions="1"                    ;;
	--rw)        imx8_reworked=1; validate_imx8_reworked      ;;
	*)           echo "Unknown option $1"; echo "usage: ${me} --help"; exit 1 ;;
	esac
	shift
done
zb_app_no_ch_no_ieee=`echo ${zb_app} | grep -c "cli_nxp\|zb_daemon\|acm_virtualtty_bridge"`
if [ -z "${channel}" ]; then
	if [ ${zb_app_no_ch_no_ieee} -gt 0 ] && [ -z "${ot_cfg}" ]; then
		echo "Channel not required for cli_nxp, zb_daemon, acm_virtualtty_bridge or ot_app without thread role"
	else
		echo "No channel provided"; echo "usage: ${me} --help"; exit 1
	fi
fi
if [ -z "${first}" ]; then
	echo "No application provided"; echo "usage: ${me} --help"; exit 1
fi
if [ -n "${ot_cfg}" ] && [ -z "${ot_net}" ]; then
	echo "For --otcfg option, need to define if we create a new network or join an existing one using --otnet <create|join>"; echo "usage: ${me} --help"; exit 1
fi
if [ ${zb_app_no_ch_no_ieee} -gt 0 ] && [ -n "${ieee_addr}" ]; then
	echo "Cannot use --ieee with cli_nxp, zb_daemon or acm_virtualtty_bridge application"; echo "usage: ${me} --help"; exit 1
fi

chipid=`grep "Bluetooth" /var/log/syslog | grep "ChipID" | tail -1 | awk -F"ChipID: " '{print $2}' | awk -F", Version" '{print $1}'`
#echo "++++chipId: ${chipid}---"
if [ -n "${chipid}" ]; then
	case ${chipid} in
		7601)      echo "IW612 detected"; chip_name=IW612 ;;
		8800|8801) echo "IW610 detected"; chip_name=IW610 ;;
		*)         echo "Unknown chipid  ${chipid}"; exit 1 ;;
	esac
elif [ -z "${chip_name}" ]; then
	# Assume it is IW612, if it is not the case, the caller needs to provide --chip IW610
	echo "IW612 expected (else use --chip option)"
	chip_name=IW612
else
	# Check chip supported
	case ${chip_name} in
		IW610|IW612) echo "${chip_name} configured" ;;
		*)           echo "Unsupported chip ${chip_name}"; exit 1 ;;
	esac
fi

me_5sum=`md5sum ${me} | awk '{print $1}' | cut -b 1-15`
if [ -n  "${firmware}" ]; then
  fw_5sum=`md5sum ${firmware} | awk '{print $1}' | cut -b 1-15`
else
  fw_5sum=""
fi

echo "me:       ${me} ${me_5sum}"
echo "version:  release 019.2503.022"
echo "channel:  ${channel}"
echo "first:    ${first}"
echo "second:   ${second}"
echo "zb_app:   ${zb_app}"
if [ "${otbr}" = "yes" ]; then
	echo "ot_app:   ${ot_app} ${ot_cfg} backbone: ${otbr_backbone}"
else
	echo "ot_app:   ${ot_app} ${ot_cfg}"
fi
echo "firmware: ${firmware} ${fw_5sum}"


# ---------- OT_APP ----------
# define tty device to use
export OT_TTY=/tmp/ttyOpenThread
export OT_CHANNEL=${channel}

# ---------- ZB_APP ----------
# define tty device to use
export MACSPLIT_TTY=/tmp/ttyZigbee
# define channel to use (except for CLI)
# cli_nxp, zb_daemon or acm_virtualtty_bridge does not requires the channel
if [ ${zb_app_no_ch_no_ieee} -eq 0 ]; then
	export MACSPLIT_CHANNEL=${channel}
fi
# define prompt to use (for multitest app), it alsoo reduce to 0.5s the delay before getting the prompt
export MACSPLIT_PROMPT="> "
# define zboss trace level & mask
if [ "${run_level}" = "release" ]; then
	echo "run_level in ${run_level} mode"
	export ZB_TRACE_LEVEL=4
	export ZB_TRACE_MASK=0x00000800
elif [ "${run_level}" = "debug" ]; then
	echo "run_level in ${run_level} mode"
	export ZB_TRACE_LEVEL=4
	export ZB_TRACE_MASK=0xffffffff
else
	echo "run_level in unknown mode"
	export ZB_TRACE_LEVEL=0
	export ZB_TRACE_MASK=0x00000000
fi
if [ -n "${ieee_addr}" ]; then
	# Optional, extra config disabled by default, keep value configured by zb_set_long_address() in ${zb_app}:MAIN()
	#export MACSPLIT_IEEE_ADDR=aa:aa:aa:aa:aa:aa:36:15
	export MACSPLIT_IEEE_ADDR=${ieee_addr}
fi


function detect_config()
{
	# Check hardware setting to adapt config
	echo ""
	soc_id=`cat /sys/devices/soc0/soc_id`

	bsp_version=`uname -r | awk -F"-g" '{print $1}'`
	kernel_version=`echo ${bsp_version} | awk -F"-" '{print $1}'`
	kernel_maj=`echo ${kernel_version} | awk -F"." '{print $1}'`
	kernel_min=`echo ${kernel_version} | awk -F"." '{print $2}'`
	kernel_idx=`echo ${kernel_version} | awk -F"." '{print $3}'`

	# Check DTB version, if applicable
	dtb_version_match=`tr -d '\0' < /proc/device-tree/model | grep -c "board for IW"`
	if [ ${dtb_version_match} -gt 0 ]; then
		dtb_version=`tr -d '\0' < /proc/device-tree/model | awk -F"board for IW" '{print $2}' | awk -F"-v" '{print $2}' | awk -F":" '{print $1}'`
		echo "Config detected: ${soc_id}, kernel ${kernel_maj}.${kernel_min}.${kernel_idx}, dtb version ${dtb_version}"
	else
		dtb_version=0
		echo "Config detected: ${soc_id}, kernel ${kernel_maj}.${kernel_min}.${kernel_idx}"
	fi

	if_eth0=`ifconfig eth0 2>/dev/null`
	if [ $? -ne 0 ]; then
		echo "Invalid Setup, dtb does not match kernel ${kernel_version}, please update it"
		exit 1
	fi

	# Backward compatibility on fw_loader_imx_lnx and fails if the hardware is not known
	case ${soc_id} in
		i.MX8MM|i.MX8MN|i.MX8MP)
			fw_dld_dev="/dev/ttymxc2"
			# kernel 6.12.x requires minimum dtb version 2
			if [[ ${kernel_maj} -eq 6  && ${kernel_min} -ge 12 ]] || [ ${kernel_maj} -gt 6 ]; then
				if [ ${dtb_version} -lt 2 ]; then
					echo "Invalid Setup, dtb is too old for kernel ${kernel_version}, please update it"
					exit 1
				fi
			fi
			# kernel 6.6 with dtb for 6.12-v2: does not boot, stuck at:
			# mmc1: SDHCI controller on 30b50000.mmc [30b50000.mmc] using ADMA
			# kernel 6.12 with dtb for 6.6-vx: does not boot, Kernel panic after:
			# Hardware name: FSL i.MX8MM EVK board (DT)

			;;
		i.MX93|i.MX91)
			fw_dld_dev="/dev/ttyLP4"
			# kernel 6.12.x requires minimum dtb version 4
			if [[ ${kernel_maj} -eq 6  && ${kernel_min} -ge 12 ]] || [ ${kernel_maj} -gt 6 ]; then
				if [ ${dtb_version} -lt 4 ]; then
					echo "Invalid Setup, dtb is too old for kernel ${kernel_version}, please update it"
					exit 1
				fi
			fi
			;;
		*)
			echo "Unknown hardware config"
			exit 1
			;;
	esac


	# Check gpio config from pin name configured in the dtb
	# if not found keep previous code for backward compatibility
	# Caution, some hardware has different configs according to BSP version

	# SPI_INT, mandatory for ot-spi or zb_mux
	if [ ${imx8_reworked} -ne 0 ]; then
		# Rework on i.MX8M Mini to improve throughput:
		# Connect SPI_INT on R1003 (fly wire to connect to it), this line is SPDIF_EXT_CLK normally used on the Audio Card CN
		gpio_int_dev=gpiochip4
		gpio_int_lin=5
		echo "i.MX8M reworked gpio_int:      ${gpio_int_dev} line ${gpio_int_lin}"
	else
		gpio_int=`gpioinfo IWxxx_NB_SPI_INT 2>/dev/null`
		if [ $? -eq 0 ]; then
		#	gpio_int=`echo "${gpio_int}" | awk -F"\t" '{print $1}'`
			gpio_int_dev=`echo "${gpio_int}" | awk -F" " '{print $1}'`
			gpio_int_lin=`echo "${gpio_int}" | awk -F" " '{print $2}'`
			echo "autodetect gpio_int:           ${gpio_int_dev} line ${gpio_int_lin}"
		else
			case ${soc_id} in
				i.MX8MM|i.MX8MN|i.MX8MP)
					# SPI_INT: EXP_IO10 IMX8 <-> GPIO20 IW61x
					gpio_int_dev=gpiochip5
					gpio_int_lin=12
					;;
				i.MX93|i.MX91)
					# SPI_INT: M2_nALERT IMX93 <-> M2_ALERT_VDDIO M.2 Board
					gpio_int_dev=gpiochip5
					gpio_int_lin=10
					;;
			esac
			echo "manual gpio_int:           ${gpio_int_dev} line ${gpio_int_lin}"
		fi
	fi

	# 15.4_IND_RESET, mandatory for ot-spi or zb_mux
	gpio_reset=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null`
	if [ $? -eq 0 ]; then
	#	gpio_reset=`echo "${gpio_reset}" | awk -F"\t" '{print $1}'`
		gpio_reset_dev=`echo "${gpio_reset}" | awk -F" " '{print $1}'`
		gpio_reset_lin=`echo "${gpio_reset}" | awk -F" " '{print $2}'`
		echo "autodetect gpio_reset:         ${gpio_reset_dev} line ${gpio_reset_lin}"
	else
		case ${soc_id} in
			i.MX8MM|i.MX8MN|i.MX8MP)
				# IND_RST_15.4: EXP_IO11 IMX8 <-> GPIO24 IW61x
				gpio_reset_dev=gpiochip5
				case ${chip_name} in
					IW612) gpio_reset_lin=13 ;;
					IW610) gpio_reset_lin=14 ;;
				esac
				;;
			i.MX93|i.MX91)
				# INT_RST_15_4: I2C PCAL6408 2EL M.2 Murata
				if [[ ${kernel_maj} -eq 6  && ${kernel_min} -ge 12 ]] || [ ${kernel_maj} -gt 6 ]; then
					# BSP 6.12.x:
					gpio_reset_dev=gpiochip0
				else
					# BSP 6.6.x:
					gpio_reset_dev=gpiochip4
				fi
				gpio_reset_lin=1
				;;
		esac
		echo "manual gpio_reset:         ${gpio_reset_dev} line ${gpio_reset_lin}"
	fi
	# Same pin for independent reset 15.4
	iw612_gpio_ind_rst_dev=${gpio_reset_dev}
	iw612_gpio_ind_rst_lin=${gpio_reset_lin}


	# IW61x_RESET, used for auto firmware download (no use action)
	# also define reset_method based on hardware & dtb version
	iw612_gpio_reset=`gpioinfo IWxxx_PD_N 2>/dev/null`
	if [ $? -eq 0 ]; then
	#	iw612_gpio_reset=`echo "${iw612_gpio_reset}" | awk -F"\t" '{print $1}'`
		iw612_gpio_reset_dev=`echo "${iw612_gpio_reset}" | awk -F" " '{print $1}'`
		iw612_gpio_reset_lin=`echo "${iw612_gpio_reset}" | awk -F" " '{print $2}'`
		echo "autodetect iw61x_gpio_reset:   ${iw612_gpio_reset_dev} line ${iw612_gpio_reset_lin}"
		reset_method="gpio"
	else
		case ${soc_id} in
			i.MX8MM|i.MX8MN|i.MX8MP)
				reset_method="button"
				;;
			i.MX93|i.MX91)
				# Activating reset using gpio behaves on IMX93 like a full IW612 reset (both BT & WiFi)
				if [ ${dtb_version} -lt 2 ]; then
					echo "WARNING: your dtb does not allow to reset ${chip_name}, please update it"
					echo ""
					reset_method="none"
				else
					reset_method="gpio"
					# PD_n_IW612: M2_nDIS1 IMX93 <-> PD_n IW612
					iw612_gpio_reset_dev=gpiochip5
					iw612_gpio_reset_lin=20
					if [ ${dtb_version} -eq 2 ]; then
						drive_spi_en="yes"
						# SPI_ENABLE: I2C PCAL6408 2EL M.2 Murata
						if [[ ${kernel_maj} -eq 6  && ${kernel_min} -ge 12 ]] || [ ${kernel_maj} -gt 6 ]; then
							# BSP 6.12.x
							iw612_gpio_spi_ena_dev=gpiochip0
						else
							# BSP 6.6.x:
							iw612_gpio_spi_ena_dev=gpiochip4
						fi
						iw612_gpio_spi_ena_lin=0
						echo "manual iw61x_gpio_spi_ena: ${iw612_gpio_spi_ena_dev} line ${iw612_gpio_spi_ena_lin}"
					else
						drive_spi_en="no"
					fi
				fi
				echo "manual iw61x_gpio_reset:   ${iw612_gpio_reset_dev} line ${iw612_gpio_reset_lin}"
				;;
		esac
	fi


	#search the spi_dev, a bit more tricky...
	spi0_fw_path=`find /sys/firmware/devicetree/base -name spi@0`
	if [ -f ${spi0_fw_path}/label ] && [ `tr -d '\0' < ${spi0_fw_path}/label` = "IWxxx_SPIDEV" ]; then
		nbdir=`echo "${spi0_fw_path}" | awk -F"/" '{print NF}'`
		let nbdir=nbdir-1
		spi0_bus_addr=`echo "${spi0_fw_path}" | cut -d '/' -f${nbdir} | cut -d '@' -f2`
		plat_spi0_path=`find /sys/devices/platform -type d -name ${spi0_bus_addr}.spi`
		spi_dev_name=`find ${plat_spi0_path} -type d -name spidev*.* | awk -F"/" '{print $NF}'`
		spi_dev="/dev/${spi_dev_name}"
		echo "autodetect spi_dev:            ${spi_dev}"
	else
		case ${soc_id} in
			i.MX8MM|i.MX8MN|i.MX8MP)
				spi_dev="/dev/spidev1.0"
				;;
			i.MX93|i.MX91)
				spi_dev="/dev/spidev0.0"
				;;
		esac
		echo "manual spi_dev:            ${spi_dev}"
	fi
}

function check_cpu_temp()
{
	mode=`cat /sys/class/thermal/thermal_zone0/mode`
	temp=`cat /sys/class/thermal/thermal_zone0/temp`
	temp_celcius=$((temp / 1000))
	temp_milli=$((temp % 1000))
	trip_point0=`cat /sys/class/thermal/thermal_zone0/trip_point_0_temp`
	trip0_celcius=$((trip_point0 / 1000))
	trip0_milli=$((trip_point0 % 1000))
	trip0_type=`cat /sys/class/thermal/thermal_zone0/trip_point_0_type`
	trip_point1=`cat /sys/class/thermal/thermal_zone0/trip_point_1_temp`
	trip1_celcius=$((trip_point1 / 1000))
	trip1_milli=$((trip_point1 % 1000))
	trip1_type=`cat /sys/class/thermal/thermal_zone0/trip_point_1_type`
	echo ""
	echo "CPU Temperature monitor ${mode}, current: ${temp_celcius}.${temp_milli}°C (${trip0_type}: ${trip0_celcius}.${trip0_milli}°C, ${trip1_type}: ${trip1_celcius}.${trip1_milli}°C)"
}

function stop_service()
{
	this_app=$1
	if [ -f /lib/systemd/system/${this_app}.service ]; then
		echo "Stop service ${this_app}"
		systemctl stop ${this_app}
	fi
}

function check_and_kill()
{
	this_app=$1

	# follow all /
	if [ `echo ${this_app} | grep -c "/"` -gt 0 ]; then
		check_and_kill `echo ${this_app} | cut -d'/' -f 2-`
		return
	fi

	this_pid=`pidof ${this_app}`
	if [ -n "${this_pid}" ]; then
		echo
		echo "Kill ${this_app}"
		killall ${this_app}
		sleep 1
	fi
}

function init()
{
	# Sanity check
	pid_otbr=`pidof otbr-agent`
	if [ -n "${pid_otbr}" ]; then
		stop_service otbr-firewall
		stop_service otbr-agent
	fi
	check_and_kill otbr-agent
	check_and_kill ot-daemon

	if [ "${otbr}" = "yes" ]; then
		# Stop other services that does Routing Advertisement
		stop_service radvd
		systemctl mask avahi-daemon.socket
		stop_service avahi-daemon
	fi

	if [ -z "${SSH_CONNECTION}" ]; then
		# DUMP_SPINEL is not allowed over uart (too much traffic) except for "4: dump last SPINEL on disconnect"
		test_dump_spinel=${DUMP_SPINEL}
		if [ ${test_dump_spinel} -ge 4 ]; then
			test_dump_spinel=$(( ${test_dump_spinel} - 4 ))
		fi
		if [ ${DUMP_SPI} -ne 0 ] || [ ${test_dump_spinel} -ne 0 ]; then
			if [ ${ZB_MUX_OUT} -eq 1 ] || [  ${ZB_MUX_OUT} -eq 3 ] ;then
				echo ""
				echo ""
				echo "ERROR: debugging SPI|SPINEL is NOT RECOMMANDED over Uart, please use ssh"
				exit 1
			fi
		else
			if [ ${DUMP_TTY} -ne 0 ]; then
				if [ ${ZB_APP_OUT} -eq 1 ] || [  ${ZB_APP_OUT} -eq 3 ] ;then
					echo ""
					echo ""
					echo "ERROR: debugging TTY is NOT RECOMMANDED over Uart, please use ssh"
					exit 1
				fi
			fi
		fi
	fi

	if [ ${zbclean} -ne 0 ]; then
		echo ""
		echo "full zigbee cleanup of previous run"
		rm -f *.log *.log.* *.console *.console.* *.nvram *.prod *.dump *.trace
		rm -rf backup
	fi
}

function purge_coredumps()
{
	if [ ${coredump} -ne 0 ]; then
		core_sigabort=`find coredumps/ -name core-*-sig_6-pid_*`
		echo ""
		for file in ${core_sigabort}; do
			echo "remove core file due to signal abort: ${file}"
			rm -f ${file}
		done
	fi
}

function config_coredump()
{
	mkdir -p ./coredumps
	echo ""
	echo "configure core in ${PWD}/coredumps"
	ulimit -c unlimited
	echo "${PWD}/coredumps/core-%e-sig_%s-pid_%p" > /proc/sys/kernel/core_pattern
	purge_coredumps
}

function cleanup_previous_zb_app()
{
	this_app=$1

	# follow all /
	if [ `echo ${this_app} | grep -c "/"` -gt 0 ]; then
		cleanup_previous_zb_app `echo ${this_app} | cut -d'/' -f 2-`
		return
	fi

	if [ "${this_app}" = "acm_virtualtty_bridge" ]; then
		res=`lsmod | grep -c g_serial`
		if [ ${res} -eq 0 ]; then
			echo "load g_serial"
			modprobe g_serial n_ports=1 > /dev/null 2>&1
		fi
	fi

	if [ ! -f ${this_app}.nvram ]; then
		return
	fi

	# preserve nvram and log of the running apps, it will be removed in case of factory reset
	if [ -f ${this_app}.log ]; then
		mv ${this_app}.log ${this_app}.log.bak
		# removes older log files (since ZB_USE_LOGFILE_ROTATE has been enabled)
		rm -f ${this_app}.log.*
	fi
	if [ -f ${this_app}.console ] && [ ${ZB_APP_OUT} -gt 1 ]; then
		mv ${this_app}.console ${this_app}.console.bak
		# removes older log files (since ZB_USE_LOGFILE_ROTATE has been enabled)
		rm -f ${this_app}.console.*
	fi

	# Clean previous run:
	rm -f ${this_app}.dump ${this_app}.log ${this_app}.console ${this_app}.prod ${this_app}.trace


	# Check if user wants to keep NVRAM files
	echo ""
	if [ ${skip_questions} -eq 0 ]; then
        read -p "Factory reset for ${this_app} [y/n] ([y] or [Enter]) ? " factory_reset
    else
        factory_reset="N"
    fi
	case $factory_reset in
		[Nn]*)
			echo "Keep settings in ${this_app}.nvram"
			mv ${this_app}.log.bak ${this_app}.log
			echo "----------------- APPLI ${this_app} restarted ------------------" >> ${this_app}.log
			if [ ${ZB_APP_OUT} -gt 1 ]; then
				mv ${this_app}.console.bak ${this_app}.console
				echo "----------------- APPLI ${this_app} restarted ------------------" >> ${this_app}.console
			fi
			;;
		[Yy]*|*)
			echo "Factory reset: ${this_app}.nvram is removed"
			rm -f ${this_app}.nvram
			rm -f ${this_app}.log.bak
			echo "----------------- APPLI ${this_app} factory reset ------------------" > ${this_app}.log
			if [ ${ZB_APP_OUT} -gt 1 ]; then
				rm -f ${this_app}.console.bak
				echo "----------------- APPLI ${this_app} factory reset ------------------" > ${this_app}.console
			fi
			;;
	esac

	if [ ${app_track_log_error} -ne 0 ]; then
		echo "Track ERROR on ${this_app}.log"
		tail -f ${this_app}.log | grep ERROR | awk -F"ERROR" '{print "[ZB_APP_ERROR"$2}' &
	fi

	# Specific apps
	if [ ${this_app} == "ota_client_zr" ]; then
		echo ""
		if [ ${skip_questions} -eq 0 ]; then
            read -p "Purge previous OTA files [y/n] ([y] or [Enter]) ? " purge_ota
        else
            purge_ota="Y"
        fi
		case $factory_reset in
			[Yy]*|*)
				echo "OTA-FILE-* are removed"
				rm -f OTA-FILE-*
				;;
		esac
	fi
}

function reset_device()
{
	case ${reset_method} in
		button)
			mlan0cnt=`ifconfig | grep -c "mlan0"`
			if [ ${mlan0cnt} -gt 0 ]; then
				echo "CAUTION: WiFi is running"
				echo "CAUTION: in case WiFi has downloaded a combo (sduart_xxx) firmware, no need to use option [--fw <IW612-firmware>]] here..."
				echo "CAUTION: in any cases, do not push Reset button, it would kill the WiFi"
            if [ ${skip_questions} -eq 0 ]; then
				read -p "Continue [enter] ?" dummy
            fi
			else
                if [ ${skip_questions} -eq 0 ]; then
                    read -p "Push Reset button of device [enter] ? " dummy
                fi
			fi
			;;
		gpio)
			mlan0cnt=`ifconfig | grep -c "mlan0"`
			if [ ${mlan0cnt} -gt 0 ]; then
				echo "CAUTION: WiFi is running"
				echo "CAUTION: in case WiFi has downloaded a combo (sduart_xxx) firmware, no need to use option [--fw <IW612-firmware>]] here..."
				echo "CAUTION: in any cases, we don't reset iw612, it would kill the WiFi"
			else
				# Trick to know if the option --chip is supported or not (introduced on version v2.x)
				gpioinfo -h | grep "\--chip" > /dev/null
				if [ $? -eq 0 ]; then
					option="--chip"
				else
					option=""
				fi
				echo "Reset IW612 (BT-15.4 & WiFi) with ${iw612_gpio_reset_dev} line ${iw612_gpio_reset_lin}"
				gpioset ${option} ${iw612_gpio_reset_dev} ${iw612_gpio_reset_lin}=0 &
				sleep 0.5 # Wait action to be done
				killall gpioset &> /dev/null
				sleep 0.5 # Keep reset active
				gpioset ${option} ${iw612_gpio_reset_dev} ${iw612_gpio_reset_lin}=1 &
				sleep 0.5 # Wait action to be done
				killall gpioset &> /dev/null
				if [ "${drive_spi_en}" = "yes" ]; then
					echo "Enable IW612 (SPI_ENA=1 & IND_RST_15_4=0)"
					gpioset ${option} ${iw612_gpio_spi_ena_dev} ${iw612_gpio_spi_ena_lin}=1 &
					sleep 0.5 # Wait action to be done
					killall gpioset &> /dev/null
				else
					echo "Enable IW612 (IND_RST_15_4=0)"
				fi
				gpioset ${option} ${iw612_gpio_ind_rst_dev} ${iw612_gpio_ind_rst_lin}=0 &
				sleep 0.5 # Wait action to be done
				killall gpioset &> /dev/null
			fi
			;;
		none)
            if [ ${skip_questions} -eq 0 ]; then
                read -p "Cannot reset the device, would you like to reboot  [y/n] ([n] or [Enter]) ? " do_reboot
            else
                do_reboot="N"
            fi
			case ${do_reboot} in
				[Yy]*) reboot ;;
				[Nn]*|*) ;;
			esac
			;;
		*)
			echo "Unknown reset_method ${reset_method}, ABORT"
			exit 1
			;;
	esac
}

function flash_firmware()
{
	# If no firmware is provided, bringup up BT in
	echo ""

	if [ -e ${fw_dld_dev} ]; then

		killall hciattach 2> /dev/null

		if [ -n  "${firmware}" ]; then
			echo "Flash ${firmware} with w_loader_imx_lnx on ${fw_dld_dev}"
			echo ""
			reset_device
			fw_loader_imx_lnx ${fw_dld_dev} 115200 0 ${firmware} 3000000
			if [ $? -ne 0 ]; then
				echo "Flash failure, ABORT"
				exit 1
			else
				echo "Wait for the firmware to start..."
				sleep 1
			fi
		fi

		hciattach ${fw_dld_dev} any -s 115200 115200 flow dtron
		sleep 1
	else
		if [ -n  "${firmware}" ]; then
			case ${chip_name} in
				IW612) firmware_name="uartspi_n61x_v1.bin.se" ;;
				IW610) # Here we need to know if the firmware is secured or not
					secured=`echo "${firmware}" | awk -F".bin" '{print $2}'`
					#echo "+++${secured}---"
					if [ "${secured}" = ".se" ]; then
						firmware_name="uartspi_iw610.bin.se"
					else
						firmware_name="uartspi_iw610.bin"
					fi
					;;
			esac
			if [ ! -h /lib/firmware/nxp/${firmware_name} ]; then
				echo "backup original firmware ${firmware_name}"
				cd /lib/firmware/nxp
				mv ${firmware_name} ${firmware_name}_original
				cd - > /dev/null
			fi

			rm -f /lib/firmware/nxp/${firmware_name}
			abs_firmware=`readlink -f ${firmware}`
			ln -s ${abs_firmware} /lib/firmware/nxp/${firmware_name}
			this_firmware=`ls -al /lib/firmware/nxp/${firmware_name} | awk -F" -> " '{print $2}'`
			echo "Flash ${firmware} with btnxpuart (symlink on /lib/firmware/nxp/${firmware_name})"
			echo ""

			res=`lsmod | grep -c btnxpuart`
			if [ ${res} -ne 0 ]; then
				echo "remove btnxpuart"
				rmmod btnxpuart
				sleep 1
			fi
			reset_device
			if [ -n "${SSH_CONNECTION}" ]; then
				tail -f -n 0 /var/log/messages | grep "Bluetooth" &
				tail_pid=$!
			else
				tail_pid=0
			fi
			kernel_lvl=`cat /proc/sys/kernel/printk |  awk '{print $1}'`
			echo 7 > /proc/sys/kernel/printk
			echo "probe btnxpuart"
			modprobe btnxpuart
			# measured timing:
			# +0.6s: request firmware
			# uartspi:
			# +2.2s: firmware downloaded
			# +4.2s: config done (baudrate & wakeup method)
			# sduart: combo
			# +3.5s:  firmware downloaded
			# +5.0s: config done (baudrate & wakeup method)
			sleep 6
			echo ${kernel_lvl} > /proc/sys/kernel/printk
			if [ ${tail_pid} -ne 0 ]; then
				kill -SIGTERM ${tail_pid}
			fi
		else
			res=`lsmod | grep -c btnxpuart`
			if [ ${res} -eq 0 ]; then
				echo "probe btnxpuart"
				modprobe btnxpuart
				sleep 3
			fi
		fi
	fi
	sleep 1
	echo "bring up hci0"
	hciconfig hci0 up
	# Enable HCI dump
	if [ ${DUMP_HCI} -ne 0 ]; then
		btmon -w hci_dumps.pcap > /dev/null &
	fi
}

apps_running=0

function start_mux()
{
	gpio_int=${gpio_int_lin}:/dev/${gpio_int_dev}
	gpio_reset=${gpio_reset_lin}:/dev/${gpio_reset_dev}
	reset_15_4_config=${reset_15_4_mode}:${reset_15_4_threshold}

	rm -rf tmp zb_mux.log zb_mux.console
	echo ""
	if [ -n "${gdb_mux}" ]; then
        if [ ${skip_questions} -eq 0 ]; then
            read -p "Start zb_mux on gdb port ${gdb_mux} [enter] ? " dummy
        fi
		gdbserver :${gdb_mux} ./zb_mux -i ${spi_dev} -o 0:/tmp/ttyOpenThread -o 2:/tmp/ttyZigbee -s -S ${spi_speed} -m 0 -c ${spi_cs_delay} -I ${gpio_int} -R ${gpio_reset} -M ${reset_15_4_config} -t ${mux_trace} &
	else
		echo "Start zb_mux"
		./zb_mux -i ${spi_dev} -o 0:/tmp/ttyOpenThread -o 2:/tmp/ttyZigbee -R ${gpio_reset} -M ${reset_15_4_config} -s -S ${spi_speed} -m 0 -c ${spi_cs_delay} -I ${gpio_int} -t ${mux_trace} &
	fi
	sleep 1

	if [ ${mux_track_log_error} -ne 0 ]; then
		echo "Track ERROR on zb_mux.log"
		tail -f zb_mux.log | grep ERROR | awk -F"ERROR" '{print "[ZB_MUX_ERROR"$2}' &
	fi
}

function stop_mux()
{
	check_and_kill zb_mux
}

function start_zb_app()
{
	check_cpu_temp

	cleanup_previous_zb_app ${zb_app}

	echo ""
	if [ -n "${gdb_app}" ]; then
        if [ ${skip_questions} -eq 0 ]; then
            read -p "Start Zigbee ${zb_app} on gdb port ${gdb_app} [enter] ? " dummy
        fi
		if [ "$second" = "ot_app" ]; then
			# if we have thread app is running in second, we must run in background
			gdbserver :${gdb_app} ./${zb_app} &
		else
			gdbserver :${gdb_app} ./${zb_app}
		fi
	else
		if [ ${skip_questions} -eq 0 ]; then
            read -p "Start Zigbee ${zb_app} [enter] ? " dummy
        fi
		if [ "$second" = "ot_app" ]; then
			# if we have thread app is running in second, we must run in background
			./${zb_app} &
		else
			apps_running=1
			while [ ${apps_running} -eq 1 ]; do
				./${zb_app}
				sleep 1
				purge_coredumps
				check_cpu_temp
				echo ""
				if [ ${is_cli_nxp} -ne 0 ]; then
					echo "Restore echo input caracters & all special caracters restored to default values"
					stty echo
					stty sane
				fi
                if [ ${skip_questions} -eq 0 ]; then
                    read -p "Restart Zigbee ${zb_app} [y/n] ([y] or [Enter]) ? " restart
                else
                    restart="N"
                fi
				case $restart in
					[Nn]*)
						break
						;;
					[Yy]*|*)
						;;
				esac
			done
		fi
	fi

	if [ "${second}" = "ot_app" ]; then
		# We are in background, add a tempo
		sleep 3
		apps_running=1
	else
		# We are running in foreground, if we are here
		echo ""
		echo "Zigbee ${zb_app} has stopped"
		apps_running=0
	fi
}

function stop_zb_app()
{
	check_and_kill ${zb_app}
	is_cli_nxp=`echo ${zb_app} | grep -c "cli_nxp"`
	if [ ${is_cli_nxp} -ne 0 ]; then
		echo "Restore echo input caracters & all special caracters restored to default values"
		stty echo
		stty sane
	fi
}

function reset_thread()
{
	ot-ctl thread stop                   > /dev/null
	ot-ctl ifconfig down                 > /dev/null
	ot-ctl factoryreset                  > /dev/null
	sleep 2
}

function check_route()
{
	echo "Check route config"
	link_prefix_dev=`ip -6 route | grep "fe80::/64" | awk -F" " '{print $3}'`
	for dev in ${link_prefix_dev}; do
#		echo "${dev} has prefix fe80::/64"
		if [ "${dev}" != "wpan0" ]; then
			echo "device ${dev} has the same link prefix as wpan0 (fe80::/64) which is ok"
		fi
	done
	global_prefix_dev=`ip -6 route | grep "${ot_prefix}" | awk -F" " '{print $3}'`
	for dev in ${global_prefix_dev}; do
#		echo "${dev} has prefix ${prefix_dev}"
		if [ "${dev}" != "wpan0" ]; then
			echo ""
			echo "WARNING: route conflict detected, device ${dev} has the same global prefix as wpan0 (${ot_prefix})"
			echo "please update your config using option --otprefix with a different prefix"
			echo ""
		fi
	done
}

function update_network()
{
	echo "Enable IP forwarding"
	echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
# IPv4 not needed since THREAD is IPv6 only
#	echo 1 > /proc/sys/net/ipv4/ip_forward
	echo 2 > /proc/sys/net/ipv6/conf/all/accept_ra
	echo "Update iptables to forward ${otbr_backbone} <-> wpan0"
#	iptables -A FORWARD -i ${otbr_backbone} -o wpan0 -j ACCEPT
#	sleep 0.5
#	iptables -A FORWARD -i wpan0 -o ${otbr_backbone} -j ACCEPT
#	sleep 0.5
	ip6tables -A FORWARD -i ${otbr_backbone} -o wpan0 -j ACCEPT
	sleep 0.5
	ip6tables -A FORWARD -i wpan0 -o ${otbr_backbone} -j ACCEPT
	sleep 0.5
	echo ""
	check_route
}

function config_thread()
{
	OT_PANID="0x12${OT_CHANNEL}"
	OT_EXTPANID="11111111222222${OT_CHANNEL}"
	OT_NETKEY="00112233445566778899aabbccddee${OT_CHANNEL}"
	OT_NETNAME="DualPan-${OT_PANID}"
# This is configured by dataset init new that should done only by the creator of the network
#	OT_MLPREFIX="FD00:0B${OT_CHANNEL}:0000:0000::"

	# ot_cfg:
	# -------
	# definitions (from https://openthread.io/guides/thread-primer/node-roles-and-types)
	# - Full Thread Device (FTD): radio always on, subs to all-routers mcast, maintains IPv6 addr mappings
	#    - Router
	#    - REED (Router Eligible End Device): can be promoted to a router
	#    - FED (Full End Device): cannot be promoted to a router
	#   associated commands: router, routerdowngradethreshold, routereligible, routeridrange, routerselectionjitter, routerupgradethreshold
	#
	# - Minimal Thread Device (MTD): don't subs to all-routers mcast, fwd all msgs to parent
	#    - MED (Minimal End Device): transceiver always on, no need to poll messages from its parent
	#    - SED (Sleepy End Device):  normally disabled, wakes on occasion to poll messages from its parent
	#
	# OT_MODE:
	# r: rx-on-when-idle
	# d: Full Thread Device
	# n: Full Network Data
	# -: no flags:
	#    * rx-off-when-idle
	#    * Minimal Thread Device
	#    * Stable Network Data
	# rdn: leader
	# -:   sleepy end device
	# rn:  REED
	#
	# OT_RTRUPTH: routerupgradethreshold
	# => the number of active routers on the Thread network partition below which a REED may decide to become a Router
	#
	# OT_RTRDOWNTH: routerdowngradethreshold
	# => the number of active routers on the Thread network partition above which an active router may decide to become a child
	#
	# OT_RTRSELJ: routerselectionjitter
	# => a random period prior to request Router ID for REED
	#
	case ${ot_cfg} in
		# FTD:
		router) OT_MODE="rdn" ; OT_RTREL="enable"  ; OT_RTRUPTH="0" ; OT_RTRDOWNTH=""  ; OT_RTRSELJ="1" ;; # if Child && ActiveRouterCount() < UpgradeThreshold then upgrade to Router
		reed)   OT_MODE="rdn" ; OT_RTREL="enable"  ; OT_RTRUPTH=""  ; OT_RTRDOWNTH="0" ; OT_RTRSELJ="1" ;; # if Router && ActiveRouterCount() > DowngradeThreshold then downgrade to REED
		fed)    OT_MODE="rdn" ; OT_RTREL="disable" ;;
		# MTD:
		med)    OT_MODE="rn"  ;;
		sed)    OT_MODE="-"   ;;
	esac

	reset_thread

	echo "Thread config ${ot_cfg}:"
	if [ ${ot_net} = "create" ]; then
		echo -n " - create network         => "       ; ot-ctl dataset init new
	fi
	echo -n " - channel               ${OT_CHANNEL} => "       ; ot-ctl dataset channel ${OT_CHANNEL}
	echo -n " - panid                 ${OT_PANID} => "         ; ot-ctl dataset panid ${OT_PANID}
	echo -n " - extpanid              ${OT_EXTPANID} => "      ; ot-ctl dataset extpanid ${OT_EXTPANID}
	echo -n " - networkkey            ${OT_NETKEY} => "        ; ot-ctl dataset networkkey ${OT_NETKEY}
	echo -n " - networkname           ${OT_NETNAME} => "       ; ot-ctl dataset networkname ${OT_NETNAME}
#	echo -n " - meshlocalprefix       ${OT_MLPREFIX} => "      ; ot-ctl dataset meshlocalprefix ${OT_MLPREFIX}
	if [ -n "${OT_RTREL}" ];     then echo -n " - routereligible           ${OT_RTREL} => "     ; ot-ctl routereligible ${OT_RTREL} ;               fi
	if [ -n "${OT_RTRUPTH}" ];   then echo -n " - routerupgradethreshold   ${OT_RTRUPTH} => "   ; ot-ctl routerupgradethreshold ${OT_RTRUPTH} ;     fi
	if [ -n "${OT_RTRDOWNTH}" ]; then echo -n " - routerdowngradethreshold ${OT_RTRDOWNTH} => " ; ot-ctl routerdowngradethreshold ${OT_RTRDOWNTH} ; fi
	echo -n " - mode                  ${OT_MODE} => "          ; ot-ctl mode ${OT_MODE}
	ot-ctl dataset commit active         > /dev/null
	if [ -n "${OT_RTRSELJ}" ];   then echo -n " - routerselectionjitter    ${OT_RTRSELJ} => "   ; ot-ctl routerselectionjitter ${OT_RTRSELJ} ;      fi
	if [ ${ot_net} = "create" ]; then
		# prefix should be configured between dataset commit active & ifconfig up, refer to:
		# https://openthread.io/guides/border-router/external-commissioning/prepare
		echo -n " - prefix                   ${ot_prefix} => "     ;ot-ctl prefix add ${ot_prefix} pasor
	fi
	ot-ctl ifconfig up                   > /dev/null
	ot-ctl thread start                  > /dev/null
	loop=1
	good=0
	while [ $loop -le 180 ]; do
		state=`ot-ctl state | grep -v "Done" | awk -F"\r" '{print $1}'` # Get rid of \r
		if [ "${prev}" != "${state}" ]; then echo "" ; prev=${state} ; good=0 ; fi
		echo -n -e "Thread state: ${state}${msg}                         \r"
		case ${state} in
#			child)  if [ "${ot_cfg}" = "router" ];  then ot-ctl state router > /dev/null; msg=", force router"; else let good++; msg=" (ok ${good})";  fi ;; # => No, the REED can be leader if started first
			child)  if [ "${ot_cfg}" = "router" ];  then ot-ctl state leader > /dev/null; msg=", force leader"; else let good++; msg=" (ok ${good})"; fi ;;
			leader) if [ "${ot_cfg}" != "router" ]; then ot-ctl state child  > /dev/null; msg=", force child" ; else let good++; msg=" (ok ${good})"; fi ;;
			router) if [ "${ot_cfg}" != "router" ]; then ot-ctl state child  > /dev/null; msg=", force child" ; else let good++; msg=" (ok ${good})"; fi ;;
		esac
		if [ "${good}" -gt 5 ]; then
			meshlocalprefix=`ot-ctl prefix meshlocal | awk -F"/" '{print $1}' | awk -F"::" '{print $1}'`
			addresses=`ot-ctl ipaddr`
			for addr in ${addresses}; do
				case ${addr} in
					fe80:*) ;; # echo "Link-Local Thread Addr: ${addr}" ;;
					fd00:*) ;; # echo "Meld-Local Thread Addr: ${addr}" ;;
					Done*)  ;;
					*) 	# Filter address based on meshhlocalprefix since it is not a real global address
						is_mlp=`echo "${addr}" | grep -c "${meshlocalprefix}"`
						if [ ${is_mlp} -eq 0 ]; then
							echo "Global Thread Address:  ---------------------------------------"
							echo "Global Thread Address:  ${addr}"
							echo "Global Thread Address:  ---------------------------------------"
						fi
						;;
				esac
			done
			echo ""
			ot-ctl dataset active | grep "Mesh Local Prefix"
			echo "DONE"
			ifconfig wpan0
			echo ""
			if [ "${otbr}" = "yes" ]; then
				update_network
			fi

			if [ -n "${otbr_ping}" ]; then
            if [ ${skip_questions} -eq 0 ]; then
                read -p "Ping 1K on wpan0 ${otbr_ping} [enter] ? " dummy
            fi
				if [ "${second}" = "zb_app" ]; then
					# Need to run it in background in case we still have to start_zb_app
					ping -I wpan0 -6 ${otbr_ping} -s 1024 &
				else
					ping -I wpan0 -6 ${otbr_ping} -s 1024
				fi
			fi
			return
		fi
		sleep 1
		let loop++
	done
	echo -e "\nTimeout getting ${ot_cfg} state :("
}

function start_ot_app()
{
	if [ -n "${ot_region}" ]; then
		OT_TTY=${OT_TTY}"?region=${ot_region}"
		region_msg=", region domain ${ot_region}"
	fi
	check_cpu_temp
	echo ""
	case ${otbr} in
		yes)
			if [ ${skip_questions} -eq 0 ]; then
                read -p "Start OpenThread BorderRouter ${ot_app} ${ot_cfg}${region_msg}, backbone ${otbr_backbone} [enter] ? " dummy
            fi
			./${ot_app} -d ${thread_log} -I wpan0 -B ${otbr_backbone} 'spinel+hdlc+uart://'${OT_TTY} trel://${otbr_backbone} &
			;;
		no|*)
			if [ ${skip_questions} -eq 0 ]; then
                read -p "Start OpenThread ${ot_app} ${ot_cfg}${region_msg} [enter] ? " dummy
            fi
			./${ot_app} 'spinel+hdlc+uart://'${OT_TTY} -d ${thread_log}  &
			;;
	esac

	if [ "${second}" = "zb_app" ]; then
		sleep 3
	fi
	apps_running=1
	sleep 2
	fwversion=`ot-ctl fwversion | grep IWX12`
	echo ""
	echo "Firmware version: ${fwversion}"
	if [ -n "${ot_cfg}" ]; then
		config_thread
	fi
}

function stop_ot_app()
{
	if [ -n "${otbr_ping}" ] && [ -n "${ot_cfg}" ]; then
		check_and_kill ping
	fi
	check_and_kill ${ot_app}
}

function stop_all()
{
	if [ ${app_track_log_error} -ne 0 ] || [ ${mux_track_log_error} -ne 0 ]; then
		check_and_kill tail
	fi
	[ -n "${second}" ] && stop_${second} || true
	stop_${first}
	stop_mux
	#  Get rid of colors (end)
	sed 's/\x06//g' -i *.console &> /dev/null
	if [ ${DUMP_HCI} -ne 0 ]; then
		check_and_kill btmon
	fi
}

function ctrl_c()
{
	echo ""
	echo "Interrupted by user..."
	stop_all
	exit 0
}

trap ctrl_c INT

check_cpu_temp
detect_config
init
stop_all
[ ${coredump} -ne 0 ] && config_coredump || true
flash_firmware
start_mux
start_${first}
[ -n "${second}" ] && start_${second} || true
while [ ${apps_running} -eq 1 ]; do
	sleep 1
done
stop_all


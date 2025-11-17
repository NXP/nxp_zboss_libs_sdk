#!/bin/sh

# Create WorkingDirectory
# Caution: cannot get rid of this since it must exist before running zb_mux/zb_app
mkdir -p /var/local/zboss

# Check if btnxpuart has been loaded & started (can blacklisted in /etc//modprobe.d/blacklist.conf)
res=`lsmod | grep -c "btnxpuart"`
if [ ${res} -eq 0 ]; then
	echo "Load btnxpuart"
	modprobe btnxpuart
fi

sleep 6

res=`hciconfig hci0 | grep -c "UP RUNNING"`
if [ ${res} -eq 0 ]; then
	echo "Bringup hci0"
	hciconfig hci0 up
	# Disable BT Sleep Mode
	hcitool -i hci0 cmd 3f 23 03 00 00
fi

# Check if otbr-agent is present & compatible with dualpan
if [ -e /etc/default/otbr-agent ]; then
	otbr_enabled=`systemctl status otbr-agent.service | grep "Loaded: " | awk -F"; " '{print $2}'`
	otbr_dev=`cat /etc/default/otbr-agent | grep "OTBR_AGENT_OPTS" | grep  -v "^#" | awk -F "'" '{print $2}' | awk -F "//" '{print $2}' | awk -F "?" '{print $1}'`
	echo "OpenThread Border Router agent detected: on device ${otbr_dev}"
	case ${otbr_dev} in
		"/tmp/ttyOpenThread")	# DUAL PAN config, do nothing
			;;
		"/dev/ttyUSB0")	# SINLE PAN config, update it
			echo "update OpenThread Border Router agent for DUAL PAN config"
			sed 's/\/dev\/ttyUSB0?uart-baudrate=115200/\/tmp\/ttyOpenThread/g' -i /etc/default/otbr-agent
			sed 's/Requires=dbus.socket/Requires=dbus.socket zb_mux.service/g' -i /usr/lib/systemd/system/otbr-agent.service
			sed 's/After=dbus.socket/After=dbus.socket zb_mux.service/g'       -i /usr/lib/systemd/system/otbr-agent.service
			# Just in case...
			sed 's/ExecStartPre=service mdns start/ExecStartPre=systemctl start mdns/g'       -i /usr/lib/systemd/system/otbr-agent.service
			systemctl daemon-reload
			if [ "${otbr_enabled}" = "enabled" ]; then
				# Cannot restart otbr-agent.service here, so check the new device config is ok & reboot
				new_dev=`cat /etc/default/otbr-agent | grep "OTBR_AGENT_OPTS" | grep  -v "^#" | awk -F "'" '{print $2}' | awk -F "//" '{print $2}' | awk -F "?" '{print $1}'`
				if [ "${new_dev}" = "/tmp/ttyOpenThread" ]; then
					# Do this verification to avoid rebooting in loop
					systemd-notify --status="reboot for otbr-agent new config..."
					reboot
				else
					# error
					systemd-notify --status="error in otbr-agent config"
					exit 1
				fi
			fi
			;;
		*)	# Unknow device
			echo "Unkown device, disable OpenThread Border Router agent"
			systemctl disable otbr-agent.service
			;;
	esac
fi

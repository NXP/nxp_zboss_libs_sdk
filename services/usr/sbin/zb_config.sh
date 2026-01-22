#!/bin/sh

# Create WorkingDirectory
# Caution: cannot get rid of this since it must exist before running zb_mux/zb_app
mkdir -p /var/local/zboss

# Check if btnxpuart has been setup to generate udev events:
if [ -f /run/udev/data/btnxpuart:serial0-0.state ]; then
	btnxpuart_state=`cat ${btnxpuart_state_file} `
	echo "btnxpuart state file detected, current state: ${btnxpuart_state}"
else
	if [ ! -f /etc/udev/rules.d/98-btnxpuart.rules ]; then
		echo "Create /etc/udev/rules.d/98-btnxpuart.rules"
		echo "ACTION==\"change\", ENV{BTNXPUART_DEV}==\"*\", ENV{BTNXPUART_STATE}==\"*\", RUN+=\"/etc/udev/scripts/btnxpuart.sh \$env{BTNXPUART_DEV} \$env{BTNXPUART_STATE}\"" > /etc/udev/rules.d/98-btnxpuart.rules
	fi

	if [ ! -f /etc/udev/scripts/btnxpuart.sh ]; then
		echo "Create /etc/udev/scripts/btnxpuart.sh"
		echo "#!/bin/bash"                                                        > /etc/udev/scripts/btnxpuart.sh
		echo ""                                                                  >> /etc/udev/scripts/btnxpuart.sh
		echo "BASE_DIR=\"/run/udev/data/btnxpuart\""                             >> /etc/udev/scripts/btnxpuart.sh
		echo "DEVICE_NAME=\"\$1\""                                               >> /etc/udev/scripts/btnxpuart.sh
		echo "EVENT_VALUE=\"\$2\""                                               >> /etc/udev/scripts/btnxpuart.sh
		echo ""                                                                  >> /etc/udev/scripts/btnxpuart.sh
		echo "if [[ -z \"\$DEVICE_NAME\" || -z \"\$EVENT_VALUE\" ]]; then"       >> /etc/udev/scripts/btnxpuart.sh
		echo "    echo \"Error: Missing environment variables\" >&2"             >> /etc/udev/scripts/btnxpuart.sh
		echo "    exit 1"                                                        >> /etc/udev/scripts/btnxpuart.sh
		echo "fi"                                                                >> /etc/udev/scripts/btnxpuart.sh
		echo ""                                                                  >> /etc/udev/scripts/btnxpuart.sh
		echo "chattr -i ${BASE_DIR}:${DEVICE_NAME}.state"                        >> /etc/udev/scripts/btnxpuart.sh
		echo "# Write the event value to the \"state\" file"                     >> /etc/udev/scripts/btnxpuart.sh
		echo "echo \"\$EVENT_VALUE\" > \"\${BASE_DIR}:\${DEVICE_NAME}.state\""   >> /etc/udev/scripts/btnxpuart.sh
		echo "chattr +i ${BASE_DIR}:${DEVICE_NAME}.state"                        >> /etc/udev/scripts/btnxpuart.sh
		echo "exit 0"                                                            >> /etc/udev/scripts/btnxpuart.sh
		chmod +x /etc/udev/scripts/btnxpuart.sh
	fi
fi

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

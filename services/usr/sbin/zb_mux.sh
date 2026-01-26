#!/bin/sh

soc_id=`cat /sys/devices/soc0/soc_id`

# Detect i.MX model
sed 's/soc_id=toConfigure/soc_id='${soc_id}'/g' -i /etc/default/zb_mux.env

# Detect iw61x model (either downloaded by Bluetooth or WiFi driver)
if [ "${iw_model}" = "toConfigure" ]; then
	chipid=`grep "Bluetooth" /var/log/syslog | grep "ChipID" | tail -1 | awk -F"ChipID: " '{print $2}' | awk -F", Version" '{print $1}'`
	card_type=`grep "card_type: " /var/log/syslog | tail -1 | awk -F"card_type: " '{print $2}' | awk -F", config block" '{print $1}'`
	if [ -n "${card_type}" ]; then
		case ${card_type} in
			SDIW612)   echo "IW612 detected"; chip_name=IW612      ;;
			SDIW610)   echo "IW610 detected"; chip_name=IW610      ;;
			*)         echo "Unknown WiFi card_type  ${card_type}" ;;
		esac
	elif [ -n "${chipid}" ]; then
		case ${chipid} in
			7601)      echo "IW612 detected"; iw_model=IW612      ;;
			8800|8801) echo "IW610 detected"; iw_model=IW610      ;;
			*)         echo "Unknown Bluetooth chipid  ${chipid}" ;;
		esac
	fi
	sed 's/iw_model=toConfigure/iw_model='${iw_model}'/g' -i /etc/default/zb_mux.env
fi

# Restart otbr-agent service if it is active
if systemctl is-active --quiet otbr-agent; then
    echo "Restart otbr-agent service"
    systemctl restart otbr-agent
fi

#Detect kernel version
kernel_version=`uname -r | awk -F"-g" '{print $1}' | awk -F"-" '{print $1}'`
kernel_maj=`echo ${kernel_version} | awk -F"." '{print $1}'`
kernel_min=`echo ${kernel_version} | awk -F"." '{print $2}'`
kernel_idx=`echo ${kernel_version} | awk -F"." '{print $3}'`

sed 's/kernel=toConfigure/kernel='${kernel_maj}.${kernel_min}.${kernel_idx}'/g' -i /etc/default/zb_mux.env

case ${soc_id} in
	i.MX8MM|i.MX8MN|i.MX8MP)
		spi_dev="/dev/spidev1.0"
		int_dev="/dev/gpiochip5"
		int_line=12
		rst_dev="/dev/gpiochip5"
		case ${iw_model} in
			IW612) rst_line=13 ;;
			IW610) rst_line=14 ;;
			*)     echo "FATAL ERROR, unknown iw_model ${iw_model}, ABORT"; exit 1 ;;
		esac
		;;
	i.MX93|i.MX91) # default config
		if [[ "$soc_id" == "i.MX93" &&
			  ( ( kernel_maj -eq 6 && kernel_min -ge 18 ) || kernel_maj -gt 6 ) ]]; then
			spi_dev="/dev/spidev2.0"
		else
			spi_dev="/dev/spidev0.0"
		fi
		int_dev="/dev/gpiochip5"
		int_line=10
		if [[ ${kernel_maj} -eq 6  && ${kernel_min} -ge 12 ]] || [ ${kernel_maj} -gt 6 ]; then
			# BSP 6.12.x:
			rst_dev="/dev/gpiochip0"
		else
			# BSP 6.6.x:
			rst_dev="/dev/gpiochip4"
		fi
		rst_line=1
		;;
	*) # default config
		echo "unsupported platform ${soc_id}, ABORT"
		exit 1
#		spi_dev="/dev/TBD"
#		int_dev="/dev/TBD"
#		int_line=TBD
#		rst_dev="/dev/TBD"
#		rst_line=TBD
		;;
esac

# Caution: cannot create here WorkingDirectory=/var/local/zboss since it must exist before running this

# Clean WorkingDirectory
rm -f /var/local/zboss/zb_mux.log
rm -f /var/local/zboss/zb_mux.console

echo "Start ZBOSS Muxer"
/usr/sbin/zb_mux -i ${spi_dev} -o 0:/tmp/ttyOpenThread -o 2:/tmp/ttyZigbee -s -S ${spi_speed} -m 0 -I ${int_line}:${int_dev} -R ${rst_line}:${rst_dev} -M ${reset_15_4_mode}:${reset_15_4_threshold} -t ${mux_trace}

#!/bin/sh

# Caution: cannot create here WorkingDirectory=/var/local/zboss since it must exist before running this

# Clean WorkingDirectory
rm -f /var/local/zboss/${ZB_APP_NAME}.log
rm -f /var/local/zboss/${ZB_APP_NAME}.console
rm -f /var/local/zboss/${ZB_APP_NAME}.dump
rm -f /var/local/zboss/${ZB_APP_NAME}.prod
rm -f /var/local/zboss/${ZB_APP_NAME}.trace

# FactoryReset
if [ "${doFactoryReset}" = "true" ]; then
	sed 's/doFactoryReset=true/doFactoryReset=false/g' -i /etc/default/zb_app.env
	echo "Factory reset ZBOSS appli ${ZB_APP_NAME}"
	echo "----------------- APPLI on_off_switch_zed factory reset ------------------" >> /var/local/zboss/${ZB_APP_NAME}.console
	rm -f /var/local/zboss/${ZB_APP_NAME}.nvram
fi


# OTA Upgrade Server specific:
if [ -n "${ZBOSS_OTA_SERVER_DIR}" ]; then
	mkdir -p ${ZBOSS_OTA_SERVER_DIR}
fi


# X86 bridge specific:
if [ "${ZB_APP_NAME}" = "acm_virtualtty_bridge" ]; then
	res=`lsmod | grep -c g_serial`
	if [ ${res} -eq 0 ]; then
		echo "load g_serial"
		modprobe g_serial n_ports=1 > /dev/null 2>&1
	fi
fi


echo "Start ZBOSS appli ${ZB_APP_NAME}"
/usr/bin/${ZB_APP_NAME}

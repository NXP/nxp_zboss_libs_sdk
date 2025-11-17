#!/bin/sh

# $1: log file name
# $2: pid of the process

log_file=$1
zb_app_pid=$2


zb_appname=`echo ${log_file} | awk -F"." '{print $1}'`
log_type=`echo ${log_file} | awk -F"." '{print $2}'`
log_index=`echo ${log_file} | awk -F"." '{print $3}'`

function transfer_to_pc()
{
	user="user_name"
	host=192.168.1.xx
	path="~/tmp"

	# The log file will be transfered to ${path}/${pid}/.

	# Note: to connect by ssh without requesting the password:
	# Create on IMX a ssh key:
	#   ssh-keygen -t rsa
	# Copy ~/.ssh/id_rsa.pub to ${host}
	#   scp ~/.ssh/id_rsa.pub ${user}@${host}:/tmp/imx-id_rsa.pub
	# Add it to ${host}:~/.ssh/authorized_keys
	#   ssh ${user}@${host}
	#       cat /tmp/imx-id_rsa.pub >> ~/.ssh/authorized_keys
	#       exit


	echo "COPY ${log_file} to ${user}@${host}:${path}/${zb_app_pid}-${log_file}"
	scp ${log_file} ${user}@${host}:${path}/${zb_app_pid}-${log_file}
}

function transfer_to_folder()
{
	folder=backup

	mkdir -p ${folder}
	echo "COPY ${log_file} to ${folder}/${zb_app_pid}-${log_file}"
	cp ${log_file} ${folder}/${zb_app_pid}-${log_file}
}


function backup_log_of_interest()
{
	folder=logs_of_interest
	date=`date +"%m%d%y-%H%M%S"`
	interesting_file=0

	mkdir -p ${folder}
	# Purge previous execution?
	if [ ${log_index} -eq 0 ]; then
		rm -f ${folder}/*
	fi

	# Add criteria to perserve the file here


	# For ZBOSS or WCS logs
	if [ `grep -c "Firmware version\|SIGTERM caught" ${log_file}` -gt 0 ]; then
		interesting_file=1
	fi
	# ...


	# For ZBOSS logs
	if [ "${log_type}" == "log" ]; then

		if [ `grep -c "ERROR" ${log_file}` -gt 0 ]; then
			interesting_file=1
		fi
		if [ `grep -c "OOM" ${log_file}` -gt 0 ]; then
			interesting_file=1
		fi
		#...

	fi

	# For WCS logs
	if [ "${log_type}" == "console" ]; then

		if [ `grep -c "HST_RESET\|DEV_BOOT" ${log_file}` -gt 0 ]; then
			interesting_file=1
		fi
		if [ `grep -c "RxNack" ${log_file}` -gt 0 ]; then
			interesting_file=1
		fi
		if [ `grep -c "Slave did reset" ${log_file}` -gt 0 ]; then
			interesting_file=1
		fi
		#...

	fi

	if [ `grep -c "zb_app_pid" ${log_file}` -gt 0 ]; then
		interesting_file=1
	fi

	#Interesting file, back it up
	if [ ${interesting_file} -ne 0 ]; then
		cp ${log_file} ${folder}/${date}-${zb_app_pid}-${log_file}
	fi

}

backup_log_of_interest
#transfer_to_folder
#transfer_to_pc


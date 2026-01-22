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
# Firmware Dump Monitor Script
# Monitors for firmware dumps and saves them automatically
#


# Default configuration
fw_dump_dir="/var/log/fw_dumps"
log_source="/var/log/kern.log"
devcoredump_path="/sys/class/bluetooth/hci0/devcoredump/data"
monitor_pid_file="/tmp/fw-dump-monitor.pid"
btmon_pid_file="/tmp/fw-dump-btmon.pid"
verbose=0

# Define patterns
PATTERNS=("cli_nxp_zczr.console*" "cli_nxp_zczr.log*" "zb_mux.log*" "zb_mux.console*")

function usage()
{
    echo "Usage: $0 [options] <start|stop|status>"
    echo ""
    echo "Options:"
    echo "  --dir <directory>      Directory to save firmware dumps (default: ${fw_dump_dir})"
    echo "  --log <logfile>        Kernel log file to monitor (default: ${log_source})"
    echo "  --verbose              Enable verbose output"
    echo "  --help                 Show this help"
    echo ""
    echo "Commands:"
    echo "  start                  Start monitoring for firmware dumps"
    echo "  stop                   Stop monitoring"
    echo "  status                 Check monitoring status"
    echo ""
    echo "Examples:"
    echo "  $0 start                           # Start with default settings"
    echo "  $0 --dir /tmp/dumps start          # Start with custom dump directory"
    echo "  $0 stop                            # Stop monitoring"
}

function log_message()
{
    if [ ${verbose} -eq 1 ]; then
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
    fi
}

function start_btmon()
{
    # Ensure dump directory exists
    echo "fw_dump_dir: ${fw_dump_dir}"
    local btmon_output="${fw_dump_dir}/hci_dumps.pcap"
    # Kill any existing btmon processes that might interfere
    pkill -f "btmon.*hci_dumps.pcap" 2>/dev/null || true

    log_message "Starting btmon with output: ${btmon_output}"

    # Start btmon in background and capture its PID
    btmon -w "${btmon_output}" > /dev/null 2>&1 &
    local btmon_pid=$!
    # Save the PID
    echo "${btmon_pid}" > "${btmon_pid_file}"

    log_message "btmon started (PID: ${btmon_pid})"
    return 0
}

function stop_btmon()
{
    # Stop btmon using PID file
    if [ -f "${btmon_pid_file}" ]; then
        local btmon_pid=$(cat "${btmon_pid_file}")
        if kill -0 "${btmon_pid}" 2>/dev/null; then
            log_message "Stopping btmon (PID: ${btmon_pid})"
            kill "${btmon_pid}" 2>/dev/null
            # Wait a bit and force kill if necessary
            sleep 1
            if kill -0 "${btmon_pid}" 2>/dev/null; then
                kill -9 "${btmon_pid}" 2>/dev/null
            fi
        fi
        rm -f "${btmon_pid_file}"
    fi
    # Also kill any btmon processes that match our pattern
    pkill -f "btmon.*hci_dumps.pcap" 2>/dev/null || true

    log_message "btmon stopped"
}

function save_fw_dump()
{
    local timestamp=$(date +"%Y%m%d_%H%M%S")
    local dump_file="${fw_dump_dir}/fw_dump_${timestamp}.bin"
    # Create dump directory if it doesn't exist
    mkdir -p "${fw_dump_dir}"
    # Check if devcoredump data exists
    if [ -f "${devcoredump_path}" ]; then
        echo ""
        echo "===================================================="
        echo "FIRMWARE DUMP DETECTED!"
        echo "Time: $(date)"
        echo "Saving to: ${dump_file}"
        echo "===================================================="

        # Copy the dump
        cp "${devcoredump_path}" "${dump_file}"

		# Copy matching files directly to fw_dump directory
		echo "Collecting additional log files..."
		local files_found=0

		for pattern in "${PATTERNS[@]}"; do
			echo "Searching for files matching pattern: $pattern"

			# Find files matching the pattern across the entire filesystem
			find / -type f -name "$pattern" 2>/dev/null | while read -r file; do
				if [ -r "$file" ]; then
					local basename_file=$(basename "$file")
					local dest_file="${fw_dump_dir}/${basename_file}_${timestamp}"
					echo "Copying: $file -> $dest_file"
					cp "$file" "$dest_file" 2>/dev/null && {
						files_found=$((files_found + 1))
						logger "NXP FW-DUMP: Collected log file $file"
					}
				fi
			done
		done
		echo "Log file collection completed. Files collected: $files_found"

        if [ $? -eq 0 ]; then
            # Get file size and basic info
            dump_size=$(stat -c%s "${dump_file}" 2>/dev/null)
            echo "Firmware dump saved successfully!"
            echo "File: ${dump_file}"
            echo "Size: ${dump_size} bytes"
            # Try to get some basic info from the dump
            if command -v hexdump >/dev/null 2>&1; then
                echo "Header (first 32 bytes):"
                hexdump -C "${dump_file}" | head -3
            fi
            # Log to syslog as well
            logger "NXP FW-DUMP: Firmware dump saved to ${dump_file} (${dump_size} bytes)"
        else
            echo "ERROR: Failed to copy firmware dump from ${devcoredump_path}"
            logger "NXP FW-DUMP: ERROR - Failed to copy firmware dump"
        fi
    else
        echo "WARNING: Firmware dump trigger detected but no data found at ${devcoredump_path}"
        logger "NXP FW-DUMP: WARNING - Dump trigger detected but no data available"
    fi
    echo "===================================================="
}

function read_dmesg()
{
    if [ -f "${log_source}" ]; then
    tail -f "${log_source}" 2>/dev/null | while IFS= read -r line; do
        # Look for the firmware dump completion message
        if echo "$line" | grep -q "FW dump complete"; then
            log_message "Firmware dump completion detected in log"
            save_fw_dump
        fi
    done
else
    # Fallback to dmesg monitoring if log file doesn't exist
    log_message "Log file ${log_source} not found, using dmesg monitoring"
    local last_check=$(date +%s)

    while true; do
        sleep 5
        local current_time=$(date +%s)

        # Check dmesg for recent firmware dump messages
        if dmesg -T | awk -v since="$last_check" '
            {
                # Extract timestamp and convert to epoch
                cmd = "date -d \"" substr($0, 2, 19) "\" +%s 2>/dev/null"
                cmd | getline timestamp
                close(cmd)

                if (timestamp >= since && /FW dump complete/) {
                    print "found"
                    exit 0
                }
            }' | grep -q "found"; then

            log_message "Firmware dump completion detected in dmesg"
            save_fw_dump
        fi

        last_check=$current_time
    done
fi
}

function start_monitor()
{
    log_message "Starting firmware dump monitor..."
    log_message "Dump directory: ${fw_dump_dir}"
    log_message "Monitoring log: ${log_source}"
    log_message "Devcoredump path: ${devcoredump_path}"

    # Create dump directory
    mkdir -p "${fw_dump_dir}"

    # Start btmon
    start_btmon

	# Check if running under systemd
    if [ -n "$INVOCATION_ID" ]; then
    echo "INVOCATION_ID: $INVOCATION_ID"
        # Running under systemd - don't daemonize
        # Direct monitoring (no background process needed)
        log_message "Monitor started under systemd, waiting for firmware dumps..."
        read_dmesg
    else
		# Check if already running
		if [ -f "${monitor_pid_file}" ]; then
			local existing_pid=$(cat "${monitor_pid_file}")
			if kill -0 "${existing_pid}" 2>/dev/null; then
				echo "Firmware dump monitor already running (PID: ${existing_pid})"
				return 1
			else
				# Stale PID file, remove it
				rm -f "${monitor_pid_file}"
			fi
		fi
		# Start monitoring in background
		(
			echo "Monitor started, waiting for firmware dumps..."
			# Use tail to follow the log file
            read_dmesg
		) &

        local monitor_pid=$!
        echo $monitor_pid > "${monitor_pid_file}"
        echo "Firmware dump monitor started (PID: ${monitor_pid})"
        log_message "Use '$0 stop' to stop monitoring"
    fi
    return 0
}

function stop_monitor()
{
    # Stop btmon first
    stop_btmon

    if [ -f "${monitor_pid_file}" ]; then
        local monitor_pid=$(cat "${monitor_pid_file}")

        if kill -0 "${monitor_pid}" 2>/dev/null; then
            echo "Stopping firmware dump monitor (PID: ${monitor_pid})..."
            # Kill the main process and its children
            pkill -P "${monitor_pid}" 2>/dev/null
            kill "${monitor_pid}" 2>/dev/null
            # Wait a bit and force kill if necessary
            sleep 2
            if kill -0 "${monitor_pid}" 2>/dev/null; then
                kill -9 "${monitor_pid}" 2>/dev/null
            fi
            rm -f "${monitor_pid_file}"
            echo "Firmware dump monitor stopped"
        else
            echo "Monitor not running (stale PID file removed)"
            rm -f "${monitor_pid_file}"
        fi
    else
        echo "Firmware dump monitor is not running"
    fi
}

function check_status()
{
    if [ -f "${monitor_pid_file}" ]; then
        local monitor_pid=$(cat "${monitor_pid_file}")

        if kill -0 "${monitor_pid}" 2>/dev/null; then
            echo "Firmware dump monitor is running (PID: ${monitor_pid})"
            echo "Dump directory: ${fw_dump_dir}"
            echo "Monitoring: ${log_source}"
            # Check btmon status
            if [ -f "${btmon_pid_file}" ]; then
                local btmon_pid=$(cat "${btmon_pid_file}")
                if kill -0 "${btmon_pid}" 2>/dev/null; then
                    echo "btmon is running (PID: ${btmon_pid})"
                    echo "HCI capture file: ${fw_dump_dir}/hci_dumps.pcap"
                else
                    echo "btmon is not running (stale PID file)"
                fi
            else
                echo "btmon status: unknown"
            fi
            # Show recent dumps if any
            if [ -d "${fw_dump_dir}" ]; then
                local dump_count=$(ls -1 "${fw_dump_dir}"/fw_dump_*.bin 2>/dev/null | wc -l)
                echo "Dumps collected: ${dump_count}"
                if [ ${dump_count} -gt 0 ]; then
                    echo "Recent dumps:"
                    ls -lt "${fw_dump_dir}"/fw_dump_*.bin 2>/dev/null | head -5
                fi
                # Show HCI capture file info if it exists
                if [ -f "${fw_dump_dir}/hci_dumps.pcap" ]; then
                    local hci_size=$(stat -c%s "${fw_dump_dir}/hci_dumps.pcap" 2>/dev/null)
                    echo "HCI capture file size: ${hci_size} bytes"
                fi
            fi
        else
            echo "Monitor not running (stale PID file found)"
            rm -f "${monitor_pid_file}"
        fi
    else
        echo "Firmware dump monitor is not running"
    fi
}

function main()
{
	# Parse command line arguments
	while [ "$#" -gt 0 ]; do
		case $1 in
			--dir)      shift; fw_dump_dir="$1" ;;
			--log)      shift; log_source="$1" ;;
			--verbose)  verbose=1 ;;
			--help)     usage; exit 0 ;;
			start)      command="start" ;;
			stop)       command="stop" ;;
			status)     command="status" ;;
			*)          echo "Unknown option: $1"; usage; exit 1 ;;
		esac
		shift
	done

	# Execute command
	case "${command}" in
		start)  start_monitor ;;
		stop)   stop_monitor ;;
		status) check_status ;;
		*)      echo "No command specified"; usage; exit 1 ;;
	esac
}

main "$@"

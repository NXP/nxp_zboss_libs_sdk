#
# Copyright 2026 NXP
#
# NXP Proprietary. This software is owned or controlled by NXP and may only be
# used strictly in accordance with the applicable license terms. By expressly
# accepting such terms or by downloading, installing, activating and/or otherwise
# using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to be
# bound by the applicable license terms, then you may not retain, install,
# activate or otherwise use the software.

cmake_minimum_required(VERSION 3.10.2)

project(ias_zone_sensor)

# Build IAS zone sensor device
add_zigbee_executable(
    NAME izs_device
    ROLE END_DEVICE
    SOURCES 
        examples/ias_zone_sensor/izs_avg_val.c
        examples/ias_zone_sensor/izs_battery.c
        examples/ias_zone_sensor/izs_device.c
        examples/ias_zone_sensor/izs_device_utils.c
        examples/ias_zone_sensor/izs_ota_osif.c
        examples/ias_zone_sensor/izs_nvram.c
        examples/ias_zone_sensor/izs_start.c
        examples/ias_zone_sensor/izs_hal_stub.c
)

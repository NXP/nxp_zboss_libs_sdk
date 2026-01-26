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

project(ota_upgrade_nxp)

# Build OTA upgrade executables
add_zigbee_executable(
    NAME ota_server_zc
    ROLE COORDINATOR
    SOURCES examples/ota_upgrade_nxp/ota_server_zc.c
)

add_zigbee_executable(
    NAME ota_client_zr
    ROLE ROUTER
    SOURCES examples/ota_upgrade_nxp/ota_client_zr.c
)

add_zigbee_executable(
    NAME ota_client_zed
    ROLE END_DEVICE
    SOURCES examples/ota_upgrade_nxp/ota_client_zed.c
)

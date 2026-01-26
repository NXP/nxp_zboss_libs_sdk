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

project(manuf_specific_cmds_and_attrs)

# Build manufacturer specific commands and attributes executables
add_zigbee_executable(
    NAME manuf_onoff_output_zc
    ROLE COORDINATOR
    SOURCES examples/manuf_specific_cmds_and_attrs/manuf_onoff_output_zc.c
)

add_zigbee_executable(
    NAME manuf_onoff_switch_zed
    ROLE END_DEVICE
    SOURCES examples/manuf_specific_cmds_and_attrs/manuf_onoff_switch_zed.c
)

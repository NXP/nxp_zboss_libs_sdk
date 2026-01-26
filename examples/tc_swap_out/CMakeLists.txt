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

project(tc_swap_out)

# Build TC swap out executables
add_zigbee_executable(
    NAME tcso_zc
    ROLE COORDINATOR
    SOURCES examples/tc_swap_out/tcso_zc.c
)

add_zigbee_executable(
    NAME tcso_zed
    ROLE END_DEVICE
    SOURCES examples/tc_swap_out/tcso_zed.c
)

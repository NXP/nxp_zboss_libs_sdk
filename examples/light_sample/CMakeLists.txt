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

project(light_sample)

# Build light sample executables
add_zigbee_executable(
    NAME light_zc
    ROLE COORDINATOR
    SOURCES 
        examples/light_sample/light_coordinator/light_zc.c
        examples/light_sample/light_coordinator/light_zc_hal.c
)

add_zigbee_executable(
    NAME bulb
    ROLE ROUTER
    SOURCES 
        examples/light_sample/dimmable_light/bulb.c
        examples/light_sample/dimmable_light/bulb_hal.c
)

add_zigbee_executable(
    NAME light_control
    ROLE END_DEVICE
    SOURCES 
        examples/light_sample/light_control/light_control.c
        examples/light_sample/light_control/light_control_hal.c
)

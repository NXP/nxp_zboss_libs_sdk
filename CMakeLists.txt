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
project(examples)

# Include the Zigbee build system
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/zigbee_build_app.cmake)
check_yocto_sdk_environment()

# ========================================
# Include all examples
# ========================================
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/onoff_server/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_cluster/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/level_control_sample/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/manuf_specific_cmds_and_attrs/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/tc_swap_out/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/multi_ep/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/distributed/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/light_sample/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/gp_proxy/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/scenes/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/ota_upgrade_nxp/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/thermostat/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/simple_gw/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/r23_new_api/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/cli_nxp/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/dualpan_nxp/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/ias_zone_sensor/CMakeLists.txt)

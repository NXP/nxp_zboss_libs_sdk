# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add lowpower feature config fragment
get_filename_component(ZBOSS_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../config REALPATH)
set(EXTRA_CONF_FILE ${ZBOSS_CONFIG_DIR}/add_lowpower.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ias_zone_sensor)

target_sources(app PRIVATE
	src/izs_avg_val.c
	src/izs_battery.c
	src/izs_device.c
	src/izs_device_utils.c
	src/izs_hal_stub.c
	src/izs_nvram.c
	src/izs_ota_osif.c
	src/izs_start.c
)
set(DEVICE_TYPE zed)

# Add zboss library
include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/CMakeLists.txt)

target_link_libraries(app PUBLIC
	       libzboss
	       libieee.mac
	       )


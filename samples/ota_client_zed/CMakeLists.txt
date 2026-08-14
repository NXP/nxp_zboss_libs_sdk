# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add ota feature config fragment
get_filename_component(ZBOSS_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../config REALPATH)
set(EXTRA_CONF_FILE ${ZBOSS_CONFIG_DIR}/add_ota.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ota_client_zed)

target_sources(app PRIVATE
	       src/ota_client_zed.c
	       )
set(DEVICE_TYPE zed)

# Add zboss library
include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/CMakeLists.txt)

target_link_libraries(app PUBLIC
	       libzboss
	       libieee.mac
	       )

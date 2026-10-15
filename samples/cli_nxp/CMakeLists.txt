# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add cli feature config fragment
get_filename_component(ZBOSS_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../config REALPATH)
set(EXTRA_CONF_FILE ${ZBOSS_CONFIG_DIR}/add_cli.conf)

# CLI zed     : west build -b frdm_rw612 modules/zboss/samples/cli_nxp -d _build/frdm_rw612/cli_nxp_zed  -p -- -DDEVICE_TYPE=zed
# CLI zczr    : west build -b frdm_rw612 modules/zboss/samples/cli_nxp -d _build/frdm_rw612/cli_nxp_zczr -p -- -DDEVICE_TYPE=zczr
# Please note : both application can not be built at the same time in the same folder

set(DEVICE_TYPE "zed" CACHE STRING "Device type: zed or zczr")

if(DEVICE_TYPE STREQUAL "zed")
    set(OVERLAY_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/conf/zed.conf)
elseif(DEVICE_TYPE STREQUAL "zczr")
    set(OVERLAY_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/conf/zczr.conf)
else()
    message(FATAL_ERROR "Unsupported DEVICE_TYPE '${DEVICE_TYPE}' (expected 'zed' or 'zczr').")
endif()

message(STATUS "cli_nxp: building for DEVICE_TYPE '${DEVICE_TYPE}'")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cli_nxp)

target_sources(app PRIVATE
	       src/cli_main.c
	       src/cli_network.c
	       src/cli_endpoint.c
	       src/cli_cluster.c
	       src/cli_zdo.c
	       src/cli_installcode.c
	       src/cli_nvram.c
	       src/cli_tcso.c
	       src/cli_tools.c
	       src/cli_config.c
	       src/cli_menu_console.c
	       )

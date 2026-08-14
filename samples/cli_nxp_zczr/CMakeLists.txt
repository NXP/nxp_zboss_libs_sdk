# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add cli feature config fragment
get_filename_component(ZBOSS_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../config REALPATH)
set(EXTRA_CONF_FILE ${ZBOSS_CONFIG_DIR}/add_cli.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cli_nxp_zczr)

target_sources(app PRIVATE
	       src/cli_main.c
	       src/cli_network.c
	       src/cli_endpoint.c
	       src/cli_cluster.c
	       src/cli_zdo.c
	       src/cli_installcode.c
	       src/cli_nvram.c
	       src/cli_tools.c
	       src/cli_config.c
	       src/cli_menu_console.c
	       )
set(DEVICE_TYPE zc)

# Add zboss library
include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/CMakeLists.txt)

target_link_libraries(app PUBLIC
	       libzboss
	       libieee.mac
	       )


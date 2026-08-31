# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(simple_gw)

target_sources(app PRIVATE
	       src/simple_gw.c
	       src/ias_cie_addon.c
	       )
set(DEVICE_TYPE zc)

# Add zboss library
include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/CMakeLists.txt)

target_link_libraries(app PUBLIC
	       libzboss
	       libieee.mac
	       )


# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(on_off_output_zc)

target_sources(app PRIVATE
	       src/on_off_output_zc.c
	       )
set(DEVICE_TYPE zc)

# Add zboss library
include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/CMakeLists.txt)

target_link_libraries(app PUBLIC
	       libzboss
	       libieee.mac
	       )


# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add ieee mac library: Interface to NBU via RPMSG
if(${CONFIG_BOARD} STREQUAL "frdm_mcxw71")
    set(CHIP_TYPE mcxw71)
    add_library(libieee.mac STATIC IMPORTED GLOBAL)
    set_target_properties(libieee.mac PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/nxp/zephyr/blobs/mcxw71/libieee/libieee-802.15.4_MAC_intf.a
    )
endif()
if(${CONFIG_BOARD} STREQUAL "frdm_mcxw72")
    set(CHIP_TYPE mcxw72)
    add_library(libieee.mac STATIC IMPORTED GLOBAL)
    set_target_properties(libieee.mac PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/nxp/zephyr/blobs/mcxw72/libieee/libieee-802.15.4_MAC_intf.a
    )
endif()
if(${CONFIG_BOARD} STREQUAL "frdm_rw612")
    set(CHIP_TYPE rw61x)
    add_library(libieee.mac STATIC IMPORTED GLOBAL)
    set_target_properties(libieee.mac PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/nxp/zephyr/blobs/rw61x/libieee/libieee-802.15.4_MAC_intf.a
    )
endif()

# Add zboss library
if(${DEVICE_TYPE} STREQUAL "zc" OR ${DEVICE_TYPE} STREQUAL "zr")
    add_library(libzboss STATIC IMPORTED GLOBAL)
    set_target_properties(libzboss PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${CHIP_TYPE}/libzboss.a
    )
endif()
if(${DEVICE_TYPE} STREQUAL "zed")
    target_compile_options(app PRIVATE -DZB_ED_ROLE)
    add_library(libzboss STATIC IMPORTED GLOBAL)
    set_target_properties(libzboss PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${CHIP_TYPE}/libzboss.ed.a
    )
endif()
if(${DEVICE_TYPE} STREQUAL "mini")
    add_library(libzboss STATIC IMPORTED GLOBAL)
    set_target_properties(libzboss PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${CHIP_TYPE}/libzboss.mini.a
    )
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${CHIP_TYPE}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/platform
)

if(CONFIG_SUPPORT_ZBOSS)
    zephyr_library_sources(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/osif/zb_osif_nvram.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/osif/zb_osif_prod_cfg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/osif/zb_osif_serial.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/zb_zephyr_power.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/zb_zephyr_ota_partition.c
   )
endif()


include(${CMAKE_CURRENT_SOURCE_DIR}/../../libs/${CHIP_TYPE}/build_cmake)
if(${KERNEL_BUILD_TYPE} STREQUAL "debug")
    target_compile_options(app PRIVATE -DDEBUG -DUSE_ASSERT)
endif()


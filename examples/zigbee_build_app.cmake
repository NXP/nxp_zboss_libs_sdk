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

# ========================================
# Zigbee Build System
# ========================================

# Configure ZBOSS include directory
if(NOT DEFINED ZBOSS_INCLUDE_DIR)
    set(ZBOSS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "ZBOSS include directory")
endif()

# ========================================
# Function to build a Zigbee executable
# ========================================
function(add_zigbee_executable)
    set(options "")
    set(oneValueArgs NAME ROLE LIBRARY)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ZB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT ZB_NAME OR NOT ZB_SOURCES)
        return()
    endif()
    
    message(STATUS "Configuring Zigbee executable: ${ZB_NAME} (${ZB_ROLE})")
    message(STATUS "\t${ZB_SOURCES}")
   
    add_executable(${ZB_NAME} ${ZB_SOURCES})
    
    # Compilation flags based on role
    set(ROLE_FLAGS "")
    if(ZB_ROLE STREQUAL "COORDINATOR")
        set(ROLE_FLAGS -DZB_COORDINATOR_ROLE -DZB_ROUTER_ROLE)
    elseif(ZB_ROLE STREQUAL "ROUTER")
        set(ROLE_FLAGS -DZB_ROUTER_ROLE)
    elseif(ZB_ROLE STREQUAL "END_DEVICE")
        set(ROLE_FLAGS -DZB_ED_ROLE)
    endif()
    
    target_compile_options(${ZB_NAME} PRIVATE 
        -O3
        ${ROLE_FLAGS}
    )
    
    # Linker flag to strip symbols
    target_link_options(${ZB_NAME} PRIVATE -s)
    
    # Include directories
    target_include_directories(${ZB_NAME} PRIVATE
        ${ZBOSS_INCLUDE_DIR}
        ${ZBOSS_INCLUDE_DIR}/platform
        ${ZBOSS_INCLUDE_DIR}/ha
        ${ZBOSS_INCLUDE_DIR}/zcl
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
    )
    
    # Select library to use
    set(ZBOSS_LIB "libzboss.a")
    if(ZB_LIBRARY)
        set(ZBOSS_LIB ${ZB_LIBRARY})
    elseif(ZB_ROLE STREQUAL "END_DEVICE")
        set(ZBOSS_LIB "libzboss.ed.a")
    endif()
    
    # Check library existence
    set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/${ZBOSS_LIB}")
    if(NOT EXISTS ${LIB_PATH})
        message(FATAL_ERROR "Library not found: ${LIB_PATH}")
    endif()
    
    # Link libraries
    find_library(READLINE_LIB readline REQUIRED)
    target_link_libraries(${ZB_NAME} PRIVATE 
        ${LIB_PATH}
        ${READLINE_LIB}
    )
    
    install(TARGETS ${ZB_NAME} RUNTIME DESTINATION bin)
endfunction()

function(check_yocto_sdk_environment)
    # Check if we're cross-compiling (Yocto build or manual SDK)
    if(CMAKE_CROSSCOMPILING)
        message(STATUS "========================================")
        message(STATUS "Cross-compilation Environment Detected")
        message(STATUS "========================================")
        message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
        message(STATUS "System processor: ${CMAKE_SYSTEM_PROCESSOR}")
        message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
        message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
        if(DEFINED ENV{SDKTARGETSYSROOT})
            message(STATUS "SDK Sysroot: $ENV{SDKTARGETSYSROOT}")
        endif()
        message(STATUS "========================================")
        return()
    endif()
    
    # If not cross-compiling, check for manual Yocto SDK
    # List of possible Yocto SDK paths (checked in order)
    set(YOCTO_SDK_PATHS
        "/opt/fsl-imx-xwayland/6.18.2-whinlatter-matter/environment-setup-armv8a-poky-linux"
        "/opt/fsl-imx-xwayland/6.1-mickledore/environment-setup-armv8a-poky-linux"
        "/opt/fsl-imx-xwayland/5.15-kirkstone/environment-setup-armv8a-poky-linux"
    )

    # Check if the environment is already sourced
    if(NOT DEFINED ENV{SDKTARGETSYSROOT})
        message(STATUS "Yocto SDK environment not detected, checking available SDKs...")
        
        # Find first available SDK
        set(SDK_FOUND FALSE)
        foreach(SDK_PATH ${YOCTO_SDK_PATHS})
            if(EXISTS ${SDK_PATH})
                set(YOCTO_SDK_PATH ${SDK_PATH})
                set(SDK_FOUND TRUE)
                message(STATUS "Found SDK: ${SDK_PATH}")
                break()
            endif()
        endforeach()
        
        # If no SDK found, show error with all possible paths
        if(NOT SDK_FOUND)
            set(ERROR_MSG "\n========================================\n")
            set(ERROR_MSG "${ERROR_MSG}ERROR: No Yocto SDK found!\n")
            set(ERROR_MSG "${ERROR_MSG}========================================\n\n")
            set(ERROR_MSG "${ERROR_MSG}Searched in the following locations:\n")
            foreach(SDK_PATH ${YOCTO_SDK_PATHS})
                set(ERROR_MSG "${ERROR_MSG}  - ${SDK_PATH}\n")
            endforeach()
            set(ERROR_MSG "${ERROR_MSG}\nPlease install a Yocto SDK or add your SDK path to YOCTO_SDK_PATHS in CMakeLists.txt\n")
            set(ERROR_MSG "${ERROR_MSG}\n========================================\n")
            message(FATAL_ERROR ${ERROR_MSG})
        endif()
        
        # SDK exists but not sourced
        message(FATAL_ERROR 
            "\n"
            "========================================\n"
            "ERROR: Yocto SDK environment not sourced!\n"
            "========================================\n"
            "\n"
            "CMake cannot source shell scripts directly.\n"
            "You must source the environment manually before running CMake.\n"
            "\n"
            "Run this command first:\n"
            "  source ${YOCTO_SDK_PATH}\n"
            "\n"
            "Then run CMake:\n"
            "  rm -rf build/\n"
            "  cmake -B build -S .\n"
            "  cmake --build build\n"
            "\n"
            "========================================\n"
        )
    endif()

    # Verify the environment is correctly set
    message(STATUS "========================================")
    message(STATUS "Yocto SDK Environment Detected")
    message(STATUS "========================================")
    message(STATUS "SDK Sysroot: $ENV{SDKTARGETSYSROOT}")
    message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
    message(STATUS "========================================")
endfunction()

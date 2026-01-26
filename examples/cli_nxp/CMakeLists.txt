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

project(cli_nxp)

# Common sources for CLI applications
set(CLI_COMMON_SOURCES
    examples/cli_nxp/cli_main.c
    examples/cli_nxp/cli_config.c
    examples/cli_nxp/cli_network.c
    examples/cli_nxp/cli_endpoint.c
    examples/cli_nxp/cli_cluster.c
    examples/cli_nxp/cli_zdo.c
    examples/cli_nxp/cli_installcode.c
    examples/cli_nxp/cli_nvram.c
    examples/cli_nxp/cli_tools.c
)

# ========================================
# Build all executables in one call
# ========================================

# Console variant sources
set(CONSOLE_SOURCES ${CLI_COMMON_SOURCES} examples/cli_nxp/cli_menu_console.c)

# Socket variant sources
set(SOCKET_SOURCES ${CLI_COMMON_SOURCES} examples/cli_nxp/cli_menu_socket.c)

# Build all CLI executables
add_zigbee_executable(NAME cli_nxp_zczr ROLE COORDINATOR SOURCES ${CONSOLE_SOURCES})
add_zigbee_executable(NAME cli_nxp_zed ROLE END_DEVICE SOURCES ${CONSOLE_SOURCES})

add_zigbee_executable(NAME zb_daemon_zczr ROLE COORDINATOR SOURCES ${SOCKET_SOURCES})
add_zigbee_executable(NAME zb_daemon_zed ROLE END_DEVICE SOURCES ${SOCKET_SOURCES})

add_zigbee_executable(NAME zb-ctl ROLE COORDINATOR SOURCES examples/cli_nxp/zb-ctl.c)

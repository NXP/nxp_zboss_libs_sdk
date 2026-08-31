# SPDX-License-Identifier: Apache-2.0
#
# prodcfg_provision - standalone one-shot IFR0 prod-config provisioning app.
# Writes the plaintext Zigbee production config (eui64 + zb_settings + zb_secured)
# into IFR0 via the Zephyr flash API. No ZBOSS is required: this app only uses
# the Zephyr flash subsystem.

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(prodcfg_provision)

target_sources(app PRIVATE src/main.c)

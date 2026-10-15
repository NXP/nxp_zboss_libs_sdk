# SPDX-License-Identifier: Apache-2.0
#
# modules/zboss/CMakeLists.txt  --  Zephyr module build entry (see zephyr/module.yml).
#
# Runs in module scope for every Zephyr build; the body is gated on CONFIG_ZBOSS.
# Paths are anchored on ${ZEPHYR_CURRENT_MODULE_DIR} (this module's root).
#
# Library selection is decoupled from CONFIG_BOARD:
#   - libs/<soc>/ groups the prebuilt .a + headers per SoC (rw612, mcxw72, ...)
#   - CHIP_TYPE is derived from the SoC (CONFIG_SOC_SERIES_* / CONFIG_SOC_*)
#   - device role comes from the CONFIG_ZBOSS_ROLE_* Kconfig choice

if(CONFIG_ZBOSS)

set(ZBOSS_MODULE_DIR ${ZEPHYR_CURRENT_MODULE_DIR})
set(ZBOSS_HAL_BLOBS_DIR ${ZEPHYR_CURRENT_MODULE_DIR}/../hal/nxp/zephyr/blobs)

# --------------------------------------------------------------------------
# 1. Map SoC -> CHIP_TYPE and locate the matching IEEE 802.15.4 MAC blob.
#    Selection is purely SoC-symbol based (no CONFIG_BOARD coupling).
# --------------------------------------------------------------------------
# SoC symbols verified against the checked-out zephyr tree:
#   RW610/RW612 -> CONFIG_SOC_SERIES_RW6XX (zephyr/soc/nxp/rw/Kconfig.soc)
#   MCXW72 part -> CONFIG_SOC_MCXW727C     (zephyr/soc/nxp/mcx/mcxw/mcxw7xx/Kconfig.soc)
#   MCXW71 part -> CONFIG_SOC_MCXW716C     (same file)
# Note: MCXW71 and MCXW72 share SoC series MCXW7XX, so the per-SoC symbol
# (SOC_MCXW727C / SOC_MCXW716C) must be used to pick the right lib bucket.
if(CONFIG_SOC_SERIES_RW6XX)
    set(CHIP_TYPE rw612)
    set(LIBIEEE_BLOB ${ZBOSS_HAL_BLOBS_DIR}/rw61x/libieee/libieee-802.15.4_MAC_intf.a)
elseif(CONFIG_SOC_MCXW727C)
    set(CHIP_TYPE mcxw72)
    set(LIBIEEE_BLOB ${ZBOSS_HAL_BLOBS_DIR}/mcxw72/libieee/libieee-802.15.4_MAC_intf.a)
elseif(CONFIG_SOC_MCXW716C)
    set(CHIP_TYPE mcxw71)
    set(LIBIEEE_BLOB ${ZBOSS_HAL_BLOBS_DIR}/mcxw71/libieee/libieee-802.15.4_MAC_intf.a)
else()
    message(FATAL_ERROR
        "zboss: unsupported SoC '${CONFIG_SOC}' (board '${CONFIG_BOARD}'). "
        "Add a CHIP_TYPE mapping keyed on the SoC symbol in modules/zboss/CMakeLists.txt.")
endif()

set(ZBOSS_LIB_DIR ${ZBOSS_MODULE_DIR}/libs/${CHIP_TYPE})

# --------------------------------------------------------------------------
# 2. IEEE 802.15.4 MAC library (interface to NBU via RPMSG on NB SoCs).
# --------------------------------------------------------------------------
add_library(libieee.mac STATIC IMPORTED GLOBAL)
set_target_properties(libieee.mac PROPERTIES IMPORTED_LOCATION ${LIBIEEE_BLOB})

# --------------------------------------------------------------------------
# 3. ZBOSS library variant, selected by device role (CONFIG_ZBOSS_ROLE_*).
# --------------------------------------------------------------------------
add_library(libzboss STATIC IMPORTED GLOBAL)
if(CONFIG_ZBOSS_ROLE_ZC OR CONFIG_ZBOSS_ROLE_ZR OR CONFIG_ZBOSS_ROLE_ZCZR)
    set_target_properties(libzboss PROPERTIES IMPORTED_LOCATION ${ZBOSS_LIB_DIR}/libzboss.a)
elseif(CONFIG_ZBOSS_ROLE_ZED)
    zephyr_compile_definitions(ZB_ED_ROLE)
    set_target_properties(libzboss PROPERTIES IMPORTED_LOCATION ${ZBOSS_LIB_DIR}/libzboss.ed.a)
elseif(CONFIG_ZBOSS_ROLE_MINI)
    set_target_properties(libzboss PROPERTIES IMPORTED_LOCATION ${ZBOSS_LIB_DIR}/libzboss.mini.a)
endif()

# Link the imported libraries into the application automatically so samples
# no longer need to name them in target_link_libraries().
zephyr_link_libraries(libzboss libieee.mac)

# --------------------------------------------------------------------------
# 4. Include paths: SoC-specific headers (zb_vendor.h, zb_cfg_macsplit_nxp.h)
#    live under libs/<soc>; common platform headers under include/.
# --------------------------------------------------------------------------
# Use zephyr_include_directories (not plain include_directories, which is
# global) so the paths attach to the Zephyr interface library in module scope.
zephyr_include_directories(
    ${ZBOSS_LIB_DIR}
    ${ZBOSS_MODULE_DIR}/include
    ${ZBOSS_MODULE_DIR}/include/platform
)

# --------------------------------------------------------------------------
# 5. Platform (osif) sources. Common sources are board-agnostic; the
#    production-config backend is selected per SoC.
# --------------------------------------------------------------------------
zephyr_library_sources(
    ${ZBOSS_MODULE_DIR}/platform/osif/zb_osif_nvram.c
    ${ZBOSS_MODULE_DIR}/platform/osif/zb_osif_serial.c
    ${ZBOSS_MODULE_DIR}/platform/zb_zephyr_power.c
    ${ZBOSS_MODULE_DIR}/platform/zb_zephyr_ota_partition.c
)

# Production-config backend, selected via Kconfig choice (ZBOSS_PROD_CFG_BACKEND
# in Kconfig; defaulted per SoC, overridable per board in boards/<board>.conf):
#   ZBOSS_PROD_CFG_FLASH -> flash partition (RW612)       -> _flash.c
#   ZBOSS_PROD_CFG_IFR0  -> IFR0 cells      (MCXW72)      -> _ifr0.c
zephyr_library_sources_ifdef(CONFIG_ZBOSS_PROD_CFG_FLASH
    ${ZBOSS_MODULE_DIR}/platform/osif/zb_osif_nvram_prod_flash.c)
zephyr_library_sources_ifdef(CONFIG_ZBOSS_PROD_CFG_IFR0
    ${ZBOSS_MODULE_DIR}/platform/osif/zb_osif_nvram_prod_ifr0.c)

# Hardware-accelerated X25519 scalar multiplication (PSA/mbedTLS -> ELS).
# Enabled by CONFIG_ZBOSS_HW_CRYPTO_SCALARMULT, which must match the
# ZB_HW_CRYPTO_SCALARMULT define in libs/<soc>/zb_vendor.h. The Kconfig option
# auto-selects the PSA/mbedTLS dependencies; here we only compile the backend
# and expose the tf-psa-crypto headers it includes ("psa/crypto.h", mbedtls/*).
if(CONFIG_ZBOSS_HW_CRYPTO_SCALARMULT)
    zephyr_library_sources(${ZBOSS_MODULE_DIR}/platform/zb_zephyr_hw_crypto_psa.c)
endif()

# --------------------------------------------------------------------------
# 6. SoC-specific build knobs.
# --------------------------------------------------------------------------
include(${ZBOSS_LIB_DIR}/build_cmake)
if(${KERNEL_BUILD_TYPE} STREQUAL "debug")
    zephyr_compile_definitions(DEBUG USE_ASSERT)
endif()

endif() # CONFIG_ZBOSS

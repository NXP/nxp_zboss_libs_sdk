# NXP ZBOSS SDK for Zephyr — Release Note

> This document is cumulative: the most recent release is at the top and the
> history of previous releases is kept below. Each release block gathers the
> change log and the known limitations for that release.

---

## Release 2026-Q4

| Item           | Value |
| -------------- | ----- |
| Platform       | Zephyr |
| Zboss Host     | 019.2604.018 |
| Zboss Libs     | 4.2.2.0 (zoi_release-4.2.2.0) |
| Supported SoCs | MCX-W72, RW612 |

### Overview

This release addresses the known limitations/issues identified in the previous 2026-Q3 release (019.2603.065).
It includes updated NBU firmware to improve the stability and reliability.

### New features

- Add the required infrastructure to support future hw_crypto API enablement on RW612
- Update cli_nxp:
  - Add a Trust Center Swap-Out (TCSO) menu to demonstrate the Trust Center Swap-Out feature
  - Add Poll Control cluster support
  - Unify cli_nxp into a single DEVICE_TYPE-selectable Zephyr sample (zed / zczr) built with -DDEVICE_TYPE

### Improvements

- Update IEEE 802.15.4 middleware and NBU binaries with energy scan improvements
- Update nxp_zboss_libs_sdk:
  - Rework zboss module structure
  - Allow per-application override of the module Kconfig

### Bug fixes

- Fix many-to-one route to coordinator corruption on ZR
- Fix loss of encrypted datasets during NVRAM page migration, preserving persistent storage across warm reboots
- Fix emission of global ClusterRevision attribute only on standard Discover Attributes responses

### Known limitations

#### RW612

> This EAR release provides partial feature support. <br>
> Some planned features are not yet available and will be introduced in future releases 


---

## Release 2026-Q3

| Item           | Value                       |
| -------------- | --------------------------- |
| Platform       | Zephyr                      |
| Zboss Host     | 019.2603.065 / 019.2603.029 |
| Zboss Libs     | 4.2.2.0                     |
| Supported SoCs | MCX-W72, RW612              |

### Overview

Two releases were delivered this quarter. **019.2603.065** enables EdgeLock
secure key management and the Router role on MCX-W72, integrates the remaining
Discover Attribute fixes and updates the Zephyr middleware to SDK release
26.06.00-lts. **019.2603.029** brings MCX-W72 to RFP and RW612 to EAR, drops
MCX-W71 support, and adds BSP 6.18.20 toolchain support.

### New features

- 019.2603.065:
  - Enable Secure EdgeLock for Zigbee key management (MCX-W72)
  - Enable the ZR role on MCX-W72
  - Add a `config/` folder with per-feature configuration files
  - Add sample apps: IAS Zone Sensor (ZED), Simple Gateway (ZC), Light sample
    Router (ZR), and `prodcfg_provision` to write the production config into
    IFR0 at runtime (MCX-W72)
  - Add Poll Control cluster support to cli_nxp
- 019.2603.029:
  - MCX-W72 RFP and RW612 EAR
  - Add cli_nxp & onoff_server Coordinator for RW612
  - Add BSP 6.18.20 (toolchain) support

### Improvements

- 019.2603.065:
  - Update IEEE 802.15.4 middleware and NBU binaries to SDK release 26.06.00-lts
  - Improve TX power management
  - Eliminate residual background activity in low-power mode in release builds
- 019.2603.029:
  - MCX-W71 is no longer supported
  - Production config read via nvmem (stored in IFR0 for MCX-W7x)
  - Use the NXP bootROM only for OTA
  - Update cli_nxp config `ed_timeout` (timeout in milliseconds)
  - Misc improvements

### Bug fixes

- 019.2603.065: integrate the remaining Discover Attribute fixes
- 019.2603.029: fix the Discover Attribute response

### Known limitations

#### Warning
2026-09-30: hal_nxp SHA updated from 0c8f49a767b205c25e63da24a801bdbeb3e2293b to 9b775f914b4998fb2e3d1f98d735f9307217882e to fix a blob retrieval issue. No Zigbee stack change.

#### MCX-W72

> Encrypted datasets are lost during NVRAM page migration <br>
A NVRAM page is 16kB large. When a page is full, dataset are written on 2nd page, and new ELE master key is generated. However, if some encrypted data are still on 1st page, it will be impossible to decrypt them anymore.
Thus, if NVRAM content is smaller that 16kB (1 page), this issue is not observed. Therefore, ZED are not impacted by this limitation.
When operating as ZR, a page change usually occurs when there is more than ~20 siblings/children neighbor devices.

> ZR Sporadic address mismatch on relayed packets <br>
Failure observed on Zigbee Compliance Test tp_pro_bv-08 due to a sporadic address mismatch on ZR side (Node Descriptor Request and Buffer Test Request packets sometimes relayed to the wrong device).

> Clusters with no attributes show Cluster revision attribute on manufacturer specific attribute discoveries <br>

---

## Release 2026-Q2

| Item           | Value           |
| -------------- | --------------- |
| Platform       | Zephyr          |
| Zboss Host     | 019.2602.049    |
| Zboss Libs     | 4.2.2.0         |
| Supported SoCs | MCX-W72, MCX-W71 |

### Overview

Adds Zephyr OTA support (MCU & NBU), moves NVRAM to NVS, introduces low power
(disabled by default) and sets R22 behavior for Zigbee 3.0 compliant devices.

### New features

- OTA support for MCU & NBU
- Introduce low power (disabled by default)
- MCX-W71 PRC
- Add production_config on the flash hw_params partition
- Add Linux BSP 6.18.2 (toolchain) support

### Improvements

- Set R22 behavior for Zigbee 3.0 compliant devices
- NVRAM uses NVS instead of LittleFS
- cli_nxp: allow attr_disc | read | write for a manufacturer code
- Allow manufacturer attributes in the 0x8xxx range
- Misc improvements

---

## Release 2026-Q1

| Item           | Value           |
| -------------- | --------------- |
| Platform       | Zephyr          |
| Zboss Host     | 019.2601.028    |
| Zboss Libs     | 4.2.2.0         |
| Supported SoCs | MCX-W72, MCX-W71 |

### Overview

Refactors the vendor files for a standardized feature set and extends MCX-W71
Zephyr support. First Zephyr enablement (EAR).

### New features

- Add MCX-W71 support for cli_nxp_zed (EAR2)
- Add MCX-W71 persistent storage (NVRAM) support on LittleFS
- Add MCX-W71 support for onoff_switch_zed (EAR1)
- cli_nxp: add config `ed_timeout` (End Device timeout with built-in keepalive
  mechanism)

### Improvements

- Refactor vendor files for improved readability and a standardized feature set
- Enable zboss logs on MCX-W71 (disabled by default)
- Update the imx-dualpan.sh script to make it independent of the DTB version

---

## Documentation

- Getting started: see the Zephyr
  [Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).
- SDK usage: see `README-zephyr.txt`.

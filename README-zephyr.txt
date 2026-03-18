+-----------------------------------------------------------------------------+
|                        ZBOSS Zephyr delivery package                        |
+-----------------------------------------------------------------------------+


 Copyright 2025-2026 NXP

 NXP Proprietary.
 This software is owned or controlled by NXP and may only be used strictly
 in accordance with the applicable license terms. By expressly accepting
 such terms or by downloading, installing, activating and/or otherwise using
 the software, you are agreeing that you have read, and that you agree to
 comply with and are bound by, such license terms. If you do not agree to be
 bound by the applicable license terms, then you may not retain, install,
 activate or otherwise use the software.



Versioning:
===========

Date:    Thu, 05 Mar 2026 08:15:12 +0000
Version: 019.2602.024
Sha1:    9c759ff
Zboss:   zoi_release-4.2.2.0


Delivery content:
=================

modules/zboss/
├── include
│   ...                              => Zboss headers files for template or custom Zigbee app
├── libs
│   └── BOARD
│       ...                          => Zboss libraries files for template or custom Zigbee app
│       └── zb_vendor.h              => Zboss config header files for template or custom Zigbee app
├── platform
│   ├── osif
│   │   ├── zb_osif_nvram.c          => Zboss Operating system Interface for NVRAM (persistant storage)
│   │   └── zb_osif_serial.c         => Zboss Operating system Interface for Serial (Zboss logs on second uart)
│   └── zb_zephyr_power.c            => Zephyr Low Power acess
├── samples
│   ├── APPLICATION                  => Zephyr Application project description
│   │   ├── CMakeLists.txt           => Zephyr CMake
│   │   ├── doc
│   │   │   ├── readme.txt           => Description of samples
│   │   │   └── test_readme.txt      => Description of samples test steps
│   │   ├── Kconfig                  => Zephyr Kconfig
│   │   ├── prj.conf                 => Zephyr project config
│   │   └── src
│   │       └── *.c/*.h
│   ├── cli_nxp_zed                  => Sample of Command Line Interface
│   │   ├── ...                      => Zephyr Application project
│   │   ├── doc                      => Zboss Application documentation
│   │   └── src
│   │       └── *.c/*.h              => Zboss Application Command Line Interface End Device
│   ├── on_off_switch_zed            => Sample of OnOff cluster
│   │   ├── ...                      => Zephyr Application project
│   │   ├── doc                      => Zboss Application documentation
│   │   └── src
│   │       └── on_off_switch_zed.c  => Zboss Application OnOff cluster End Device
│   └── ota_client_zed               => Sample of Ota Upgrade cluster
│       ├── ...                      => Zephyr Application project
│       ├── doc                      => Zboss Application documentation
│       └── src
│           ├── ota_client.h         => Zboss Definition Ota Upgrade cluster End Device
│           └── ota_client_zed.c     => Zboss Application Ota Upgrade cluster End Device
...
└── zboss-api-user-guide
    └── index.html                   => Zboss Api documentation (html format)


Overview:
=========

- Documentation
- Install tools
	- SPSDK (flash NBU)
	- JLinkExe (flash MCU)
- Fetch Zboss on Zephyr
- Zigbee examples
	- Configuration
	- Compilation
	- MCUBoot
	- Deployment & execution
	- Debugging
	- Low power
	- Zboss Logs
- Run Zigbee applications
	- cli_nxp
	- onoff_server
	- ota_upgrade_nxp
- Tools setup
	- Wireshark
- Misc topics
	- Manufacturer code
	- Memory buffer handling


Documentation:
==============

FRDM-MCXW71 — Zephyr Project Documentation:
https://docs.zephyrproject.org/4.2.0/boards/nxp/frdm_mcxw71/doc/index.html

FRDM-MCXW72 — Zephyr Project Documentation:
https://docs.zephyrproject.org/4.2.0/boards/nxp/frdm_mcxw72/doc/index.html


SPSDK (flash NBU):
https://spsdk.readthedocs.io/en/latest/examples/_knowledge_base/installation_guide.html


Install tools:
==============

SPSDK (flash NBU):
------------------

Linux:
  python3 -m venv venvsource venv/bin/activate
  python -m pip install --upgrade pip
  pip install spsdk
  blhost --help

Windows:
  python -m venv venv
  venv\Scripts\activate
  python -m pip install --upgrade pip
  pip install spsdk
  blhost --help


JLinkExe (flash MCU):
---------------------

Install J-Link:
https://www.segger.com/downloads/jlink/

Tips:
To avoid getting everyday a popup "Terms of use" and select automatically 
"Do not show this message again today", uppdate the field "LicenseMCULink_DontShowAgainToday"
in ~/.config/SEGGER/SEGGER_REG_HKEY_CURRENT_USER.xml:

datehex=`printf '%04X%02X%02X\n' $(date +%Y) $(date +%m) $(date +%d)`
sed "s/LicenseMCULink_DontShowAgainToday=\"0x[0-9A-Fa-f]\{8\}\"/LicenseMCULink_DontShowAgainToday=\"0x${datehex}\"/g" -i ~/.config/SEGGER/SEGGER_REG_HKEY_CURRENT_USER.xml


Fetch Zboss on Zephyr:
======================

mkdir -p nxp_zephyr_zboss
cd nxp_zephyr_zboss
west init -m https://github.com/NXP/nxp_zboss_libs_sdk.git . --mr release/zephyr
west update


Temporary patch on zephyr for OTA support:
------------------------------------------

cd zephyr
git am ../modules/zboss/zephyr-mcuboot-nbu.patch
cd -


Zigbee samples:
===============

The modules/zboss/samples folder provides source examples of Zigbee End device.


Configuration:
--------------

The channel is configured by the API, zb_set_network_ed_role(1l<<channel_number) in the MAIN() function


Compilation:
------------

Go to Zephyr, activate it:
    source ~/zephyrproject/.venv/bin/activate

Update hal_nxp blobs (for libieee & nbu firmware):
    west blobs fetch hal_nxp

Compile sample application:
    west build -b frdm_mcxw71 modules/zboss/samples/<app_name> -d _build/frdm_mcxw71/<app_name> -p
    west build -b frdm_mcxw72 modules/zboss/samples/<app_name> -d _build/frdm_mcxw72/<app_name> -p


MCUboot:
--------

Note: MCXW71 allows application image max size limited to 424KB.
In case your application is bigger, external flash needs to be used, allowing image size upto 768 KB.
For that, MCUboot needs to use mcuboot_mcxw71_ext_flash.overlay and application needs to use app_mcxw71_ext_flash.overlay.

MCU Boot compilation:
    west build -b frdm_mcxw71 bootloader/mcuboot/boot/zephyr -d _build/frdm_mcxw71/mcuboot -p \
      -- -DOVERLAY_CONFIG=../../../../modules/zboss/configs/bootloader.conf -DDTC_OVERLAY_FILE=../../../../modules/zboss/configs/mcuboot_mcxw71_ext_flash.overlay
    west build -b frdm_mcxw72 bootloader/mcuboot/boot/zephyr -d _build/frdm_mcxw72/mcuboot -p \
      -- -DOVERLAY_CONFIG=../../../../modules/zboss/configs/bootloader.conf


Deployment & execution:
-----------------------

Flash NBU:
  To flash the NBU, the board needs to be rebooted in ISP (in-system programming) mode:
    press & hold BOOT_CONFIG (SW3)
    press & release RESET (SW1)
    release BOOT_CONFIG (SW3)

  Linux:
    # device is /dev/ttyACM#
  Windows:
    # device is COM#: device "JLink CDC UART Port"

  MCXW71:
    # Firmware is modules/hal/nxp/zephyr/blobs/mcxw71/mcxw71_nbu_ble_15_4_dyn_mac.sb3
    blhost -p <device> -- get-property 1
    blhost -p <device> flash-erase-all 2
    blhost -p <device> receive-sb-file mcxw71_nbu_ble_15_4_dyn_mac.sb3

  MCXW72:
    # Firmware is modules/hal/nxp/zephyr/blobs/mcxw72/mcxw72_nbu_ble_15_4_dyn_mac.bin
    blhost -p <device> -- get-property 1
    blhost -p <device> flash-erase-all 2
    blhost -p <device> write-memory 0x48800000 mcxw72_nbu_ble_15_4_dyn_mac.bin


Flash & control MCU:
  Note: if the low power (CONFIG_PM) is enabled on the prj.conf of the application, JLinkExe cannot connect,
        please enter to ISP Mode to be able to connect

  JLinkExe (enter ISP Mode if CONFIG_PM=y)
    connect
      Device>   specify MCXW716 (for FRDM-MCXW71)
      Device>   specify MCXW727C_M33_0 (for FRDM-MCXW72)
      TIF>      specify S: SWD
      JTAGConf> specify default
      Speed>    specify default
    erase
    loadfile _build/frdm_<board>/<zb_app>/zephyr/zephyr.elf
    reset
    go

  Note:
  In case MCUBoot is involved, we do not flash _build/frdm_<board>/<zb_app>/zephyr/zephyr.elf anymore:
  - MCUBoot is flashed in boot_partition
  - zb_app_signed is flashed in slot0_partition
  Partitions' offset are defined by zephyr/boards/nxp/frdm_<board>/frdm_<board>.dts -> flash -> partitions

  MCXW71:
    loadfile _build/frdm_<board>/mcuboot/zephyr/zephyr.bin         0x00000000
    loadfile _build/frdm_<board>/<zb_app>/zephyr/zephyr.signed.bin 0x00010000

  MCXW72:
    loadfile _build/frdm_<board>/mcuboot/zephyr/zephyr.bin         0x00000000
    loadfile _build/frdm_<board>/<zb_app>/zephyr/zephyr.signed.bin 0x00014000


Erase Flash:
        The flash can be erased over ISP mode using blhost command (NBU firmwre is preserved):
        blhost -p </dev/ttyACM|COM># flash-erase-all 0


Debugging:
----------

Exchange between the host and the NBU can be monitored using patch modules/zboss/libs/hal_nxp_debug_libieee.patch:
  cd hal/nxp
  patch -p1 < ../../../../zboss/libs/hal_nxp_debug_libieee.patch
=> the recompile the application.
When done, remove the patch:
  cd hal/nxp
  git checkout mcux/middleware/mcux-sdk-middleware-ieee_802.15.4/ieee_802_15_4/mac/source/App/mac_intf_rpmsg.c
  git checkout mcux/middleware/mcux-sdk-middleware-ieee_802.15.4/ieee_802_15_4/phy/source/SerialDevice/Phy.c

This patch will log RPMSG (MAC & PHY) on the console.

Note: this patch increase significatly the number of logs and slow down the execution.


Low power:
----------

By default, Zephyr Low Power is disabled.
To enable it, please uncomment "CONFIG_PM=y" in prj.conf of modules/zboss/samples/<app_name>

Low power does allow JLinkExe connection, refer to "Flash & control MCU"


Zboss Logs:
-----------

The Zboss stack provides logs in binary format on second uart, it is disabled by default.
To enable it, in modules/zboss/platform/osif/zb_osif_serial.c, change 
  #undef SEND_ZBOSS_LOGS_ON_UART 
by 
  #define SEND_ZBOSS_LOGS_ON_UART

CAUTON: The Zboss logs slow down the execution of the stack (need to wait for the data to be sent over the uart).
        It impacts performance of the stack.

- hardware settings:

  connect a FTDI TTL-232R-3V3 to the board:
  - FRDM_MCXW7x: Tx on J1-1, Rx on J1-2
  - EVLK_MCXW7x: Tx on J16-3, Rx on J17-3


- live debugging (requires source code of the stack):

  cd <path to zboss_core>
  rm -f platform/stack
  cd samples; ln -s ../../nxp_addons/examples nxp_examples; cd -
  devtools/win_com_dump/lin_com_dump_builds/ubuntu_x64/lin_com_dump -U /dev/ttyUSB# <app_name>.log <app_name>.dump &
  tail -f <app_name>.log


- record debbuging (no source code of the stack):

  sudo cat /dev/ttyUSB# > <app_name>.cap
  => each log starts by (in hexa) de ad <len> 02 then len-2 bytes


- play recorded logs (requires source code of the stack):

  cd <path to zboss_core>
  rm -f platform/stack
  cd samples; ln -s ../../nxp_addons/examples nxp_examples; cd -
  devtools/win_com_dump/lin_com_dump_builds/ubuntu_x64/lin_com_dump -B <app_name>.cap <app_name>.log <app_name>.dump
  gedit <app_name>.log


Run Zigbee application:
=======================

cli_nxp usecase (ZC/ZR, ZED):
-----------------------------

Start a Zigbee Coordinator and open the network
Start MCXW7x board

Each MCXW7x prompts "zbcli>"

Refer to docs/readme-example-cli_nxp.txt

Note: Low Power:
cli_nxp does not support Low Power due to the console from uart


onoff_server usecase (ZED):
---------------------------

Start a Zigbee Coordinator and open the network
Start MCXW7x board

Once Switch (ZED) is discovered, Output (ZC) starts periodical 
On-Off-On-Off-...


ota_upgrade_nxp usecase (ZED):
------------------------------

Flash MCUBoot zephyr.elf and signed ota_client_zed on MCXW7x board

Start a Zigbee Coordinator or Router with OTA Server cluster
Start MCXW7x board

Once OTA client (ZED) has joined NWK, it sends Query Next Image Request to OTA server (ZC) and the OTA upgrade is started.
When completed, the ZED switches to the new image on the next auto-reboot of the board.

Note: On MCXW71, external flash overlay is used, refer to MCUBoot compilation note.

Note: On ZC side, the OTA Server shall have the following manufacturer - image_type:
- 1037 - 240E: ota_client_zed signed.bin for MCXW71
- 1037 - 340E: ota_client_zed signed.bin for MCXW72


Tools setup:
============

Wireshark:
----------

To be able to sniff Zigbee R23, wireshark needs to have the following commit:
"ZigBee: Add dissectors for r23 stuff", available from v4.1.0rc0

https://www.wireshark.org/download.html
=> get release 4.2.0 or higher


Misc topics:
============

Manufacturer code:
------------------

Default manufaturer code in node descriptor is 0x1037 (NXP).
It can be updated using the API zb_set_node_descriptor_manufacturer_code_req() in
zboss_signal_handler() case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY.


Memory buffer handling:
-----------------------

An endpoint callback can be registered during endpoint creation which is used to handle incoming packets.
If one is registered, then there is 2 possiblities:
-return FALSE: It means stack will handle command and process it on its side.
-return TRUE: It means that stack will not do anything and it's up to the app to handle command.

CAUTION: In case TRUE is returned, any allocated buffer used to process and to handle commands must be freed after used, if not, it leads to buf memory leak.


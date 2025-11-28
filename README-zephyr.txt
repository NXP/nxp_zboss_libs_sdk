+-----------------------------------------------------------------------------+
|                        ZBOSS Zephyr delivery package                        |
+-----------------------------------------------------------------------------+


 Copyright 2025 NXP

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

Date:    Fri, 05 Dec 2025 12:36:10 +0000
Version: 019.2601.007
Sha1:    b92db0b
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
├── samples
│   ├── APPLICATION                  => Zephyr Application project description
│   │   ├── CMakeLists.txt           => Zephyr CMake
│   │   ├── doc
│   │   │   ├── readme.txt           => Description of samples
│   │   │   └── test_readme.txt      => Description of samples test steps
│   │   ├── Kconfig                  => Zephyr Kconfig
│   │   ├── prj.conf                 => Zephyr project config
│   │   └── src
│   │       └── *.c
│   └── on_off_switch_zed            => Sample of OnOff cluster
│       ├── ...                      => Zephyr Application project
│       └── src
│           └── on_off_switch_zed.c  => Zboss Application OnOff cluster End Device
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
	- Deployment & execution
	- Debugging
- Running Zigbee applicaions
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


Fetch Zboss on Zephyr:
======================

mkdir -p nxp_zephyr_zboss
cd nxp_zephyr_zboss
west init -m https://github.com/NXP/nxp_zboss_libs_sdk.git . --mr release/zephyr
west update
west blobs fetch hal_nxp


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
    west build -b frdm_mcxw71               modules/zboss/samples/on_off_switch_zed -d _build/frdm_mcxw71/on_off_switch_zed -p
    west build -b frdm_mcxw72/mcxw727c/cpu0 modules/zboss/samples/on_off_switch_zed -d _build/frdm_mcxw72/on_off_switch_zed -p


Deployment & execution:
-----------------------

Flash NBU:
  To flash the NBU, the board needs to be rebooted in ISP (in-system programming) mode:
    press & hold BOOT_CONFIG (SW3)
    press & release RESET (SW1)
    release BOOT_CONFIG (SW3)
  Linux:
    blhost -p /dev/ttyACM# receive-sb-file modules/hal/nxp/zephyr/blobs/<board>/<board>_nbu_ble_15_4_dyn_mac.<sb3|bin>
  Windows:
    COM#: device "JLink CDC UART Port"
    blhost -p COM# receive-sb-file <board>_nbu_ble_15_4_dyn_mac.sb3

Flash & control MCU:
  JLinkExe
    connect
      Device>   specify MCXW716 (for FRDM-MCXW71)
      TIF>      specify S: SWD
      JTAGConf> specify default
      Speed>    specify default
   erase
    loadfile _build/frdm_<board>/<zb_app>/zephyr/zephyr.elf
    reset
    go


Debugging:
----------

Exchange between the host and the NBU can be monitored using patch modules/zboss/libs/hal_nxp_debug_libieee.patch.
This patch needs to be applied to module/hal/nxp
This will log RPMSG (MAC & PHY) on the console.


Run Zigbee application:
=======================

onoff_server usecase (ZED):
---------------------------

Start a Zigbee Coordinator and open the network
Start MCXW7x board

Once Switch (ZED) is discovered, Output (ZC) starts periodical 
On-Off-On-Off-...


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


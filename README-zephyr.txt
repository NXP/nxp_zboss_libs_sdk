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

Date:    Thu, 20 Aug 2026 14:52:36 +0000
Version: 019.2603.065
Sha1:    6b7be4e
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
│   │   ├── zb_osif_prod_cfg.c       => Zboss Operating system Interface for PRODUCTION_CONFIG (factory settings)
│   │   └── zb_osif_serial.c         => Zboss Operating system Interface for Serial (Zboss logs on second uart)
│   ├── zb_zephyr_ota_partition.c    => Zephyr OTA upgrade partition access
│   └── zb_zephyr_power.c            => Zephyr Low Power access
├── samples
│   ├── APPLICATION                  => Zephyr Application project description
│   │   ├── CMakeLists.txt           => Zephyr CMake
│   │   ├── doc
│   │   │   ├── readme.txt           => Description of samples
│   │   │   └── test_readme.txt      => Description of samples test steps
│   │   ├── Kconfig                  => Zephyr Kconfig
│   │   ├── prj.conf                 => Zephyr project config
│   │   ├── sample.yaml              => Tools for debugging
│   │   └── src
│   │       └── *.c/*.h
│   │
│   ├── cli_nxp_zed                  => Sample of Command Line Interface End Device
│   ├── cli_nxp_zczr                 => Sample of Command Line Interface Coordinator / Router
│   ├── on_off_switch_zed            => Sample of OnOff cluster End Device
│   ├── on_off_output_zc             => Sample of OnOff cluster Coordinator
│   ├── light_control                => Sample of Light End Device
│   ├── bulb                         => Sample of Light Router
│   ├── light_zc                     => Sample of Light Coordinator
│   ├── ias_zone_sensor              => Sample of Intruder Alarm System zone sensor device
│   ├── simple_gw                    => Sample of Coordinator
│   ├── minimal_zed                  => Sample of Minimal application
│   ├── multi_ep_zed                 => Sample of Multiple Endpoints application
│   └── ota_client_zed               => Sample of Ota Upgrade cluster
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
	- Low power
	- Zboss Logs
- Run Zigbee applications
	- cli_nxp
	- minimal_zed
	- multi_ep_zed
	- onoff_server
	- ota_upgrade_nxp
	- light_sample
	- simple_gw & ias_zone_sensor
- Tools setup
	- Wireshark
- Misc topics
	- Manufacturer code
	- Memory buffer handling


Documentation:
==============

FRDM-MCXW72 — Zephyr Project Documentation:
https://docs.zephyrproject.org/4.2.0/boards/nxp/frdm_mcxw72/doc/index.html

FRDM-RW612 — Zephyr Project Documentation:
https://docs.zephyrproject.org/latest/boards/nxp/frdm_rw612/doc/index.html

SPSDK (flash MCXW7x NBU):
https://spsdk.readthedocs.io/en/latest/examples/_knowledge_base/installation_guide.html


Install tools:
==============

SPSDK (flash MCXW7x NBU):
-------------------------

Linux:
    python3 -m venv venvsource venv/bin/activate
    python3 -m pip install --upgrade pip
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
west init -m https://github.com/NXP/nxp_zboss_libs_sdk . --mr release/zephyr
west update


Update Zboss version:
---------------------

cd modules/zboss
git checkout release/zephyr
git pull
cd -
west update


Update Zephyr SDK:
------------------

cat zephyr/SDK_VERSION

If this version has not been installed:

    source ~/zephyrproject/.venv/bin/activate
    cd zephyr
    west sdk install -d ~/zephyr-sdk-<SDK_VERSION>
    cd -
    cd ~/zephyr-sdk/zephyr-sdk-<SDK_VERSION>
    ./setup.sh
    => anwser y to all questions
    => CAUTION: setup.sh needs to be re-run each time we change the version of zephyr-sdk

    sudo mkdir -p /opt/zephyr-sdk
    cd /opt/zephyr-sdk
    sudo ln -s ~/zephyr-sdk-<SDK_VERSION> .


Zigbee samples:
===============

The modules/zboss/samples folder provides source examples of Zigbee application.

sample's scope:
---------------
                               +--------+--------+
                               | MCXW72 | RW612  |
    +-------------------+------+--------+--------+
    | cli_nxp_zczr      | ZCZR |    y   |    y   |
    | cli_nxp_zed       | xED* |    y   |    y   |
    | minimal_zed       | SED  |    y   |    y   |
    | multi_ep_zed      | ZED  |    y   |    y   |
    | on_off_output_zc  | ZC   |        |    y   |
    | on_off_switch_zed | SED  |    y   |    y   |
    | ota_client_zed    | ZED  |    y   |        |
    | light_zc          | ZC   |        |    y   |
    | bulb              | ZR   |    y   |    y   |
    | light_control     | ZED  |    y   |    y   |
    | simple_gw         | ZC   |        |    y   |
    | ias_zone_sensor   | SED  |    y   |    y   |
    +-------------------+------+--------+--------+

Notes:
- SED: Sleepy End Device has the extra option low power (CONFIG_PM) in their prj.conf.
- xED*: cli_nxp can do SED but wihout CONFIG_PM enabled since it gets commands from the uart.


Configuration:
--------------

The channel is configured by the API, zb_set_network_ed_role(1l<<CONFIG_ZIGBEE_CHANNEL) in the MAIN() function
where CONFIG_ZIGBEE_CHANNEL is defined in the prj.conf of the application

Compilation:
------------

Go to Zephyr, activate it:
    source ~/zephyrproject/.venv/bin/activate

Update hal_nxp blobs (for libieee & nbu firmware):
    rm -rf modules/hal/nxp/zephyr/blobs
    west blobs fetch hal_nxp

Configure Zephyr SDK to use:
    export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
    export ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk/zephyr-sdk-`cat zephyr/SDK_VERSION`

Compile sample application:
    west build -b frdm_mcxw72 modules/zboss/samples/<app_name> -d _build/frdm_mcxw72/<app_name> -p
    west build -b frdm_rw612  modules/zboss/samples/<app_name> -d _build/frdm_rw612/<app_name>  -p


Deployment & execution:
-----------------------

Flash MCXW7x NBU:
  To flash the NBU, the board needs to be rebooted in ISP (in-system programming) mode:
    press & hold BOOT_CONFIG (SW3)
    press & release RESET (SW1)
    release BOOT_CONFIG (SW3)

  Linux:
    # device is /dev/ttyACM#
  Windows:
    # device is COM#: device "JLink CDC UART Port"

  MCXW72:
    # Firmware is modules/hal/nxp/zephyr/blobs/mcxw72/mcxw72_nbu_dyn_reduced.bin
    blhost -p <device> -- get-property 1
    blhost -p <device> flash-erase-all 2
    blhost -p <device> write-memory 0x48800000 mcxw72_nbu_dyn_reduced.bin

    # Secured binary: 
    blhost -p <device> -- get-property 1
    blhost -p <device> receive-sb-file PATH_TO_MCXW72_SECURED_BIN_GENERATED

    Note: if the write-memory command with a .bin NBU fails directly (0%), the chip is likely in "Closed NBU" state.
      This prevents unsigned firmware from being flashed. That means .bin cannot be flashed;
      the chip requires an sb3 file using the "Secured binary" procedure.
      To contain NBU image in SB3 format with custom keys using the MCUXpresso Secure Provisioning Tool, refer to the following link:
      https://docs.mcuxpresso.nxp.com/secure/latest/06_processor_specific_workflow.html#update-nbu-firmware-using-custom-sb-file
      CAUTION: On the final step, the software implicitly burns fuses with the keys of the image that was generated, this operation cannot be undone.
      Make sure the used keys are saved.


Flash RW612 NBU:
  NBU firmware is linked inside the MCU image and is flashed by the MCU during its initialization.


Flash & control MCU:

  JLinkExe
    connect
      Device>   specify MCXW727C_M33_0 (for FRDM-MCXW72)
      Device>   specify RW612 (for FRDM-RW612)
      TIF>      specify S: SWD
      JTAGConf> specify default
      Speed>    specify default
    erase
    loadfile _build/frdm_<board>/<zb_app>/zephyr/zephyr.elf
    reset
    go

  Note concerning MCXW72:
    In case of:
      - Closed NBU (.sb3): method of flashing is "Secured binary".
      - Open NBU  (.bin): JLink's erase command erases the NBU, so it needs to be reflashed as well:
          loadbin modules/hal/nxp/zephyr/blobs/mcxw72/mcxw72_nbu_dyn_reduced.bin 0x48800000


Erase Flash:
        The flash can be erased over ISP mode using blhost command (NBU firmwre is preserved):
        blhost -p </dev/ttyACM|COM># flash-erase-all 0


Debugging MCXW7x:
-----------------

Exchange between the host and the MCXW7x-NBU can be monitored using patch modules/zboss/libs/hal_nxp_debug_libieee.patch:
  cd hal/nxp
  patch -p1 < ../../../../zboss/libs/hal_nxp_debug_libieee.patch
=> the recompile the application.
When done, remove the patch:
  cd hal/nxp
  git checkout mcux/middleware/mcux-sdk-middleware-ieee_802.15.4/ieee_802_15_4/mac/source/App/mac_intf_rpmsg.c
  git checkout mcux/middleware/mcux-sdk-middleware-ieee_802.15.4/ieee_802_15_4/phy/source/SerialDevice/Phy.c

This patch will log RPMSG (MAC & PHY) on the console.

Note: this patch increase significatly the number of logs and slow down the execution.


Zboss Logs:
-----------

The Zboss stack provides logs in binary format on second uart, it is disabled by default.
Zboss logs are enabled by setting the value of CONFIG_ZBOSS_LOGS_ON_UART in prj.conf

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

Start RW612 board running cli_nxp_zczr or start a Zigbee Coordinator and open the network
Start NXP board running cli_nxp_zed or start a Zigbee End Device and join the network

Each NXP board prompts "zbcli>"

Refer to docs/readme-example-cli_nxp.txt

Note: Low Power:
cli_nxp does not support Low Power due to the console from uart


minimal usecase (ZED):
----------------------

Start a Zigbee Coordinator and open the network
Start NXP board

Minimal application just associate


multiple endpoints usecase (ZED):
---------------------------------

Start a Zigbee Coordinator and open the network
Start NXP board

Multiple endpoints application associate and provide a layout
with multiple endpoints and include some manufacturer specific
attributes


onoff_server usecase (ZC/ZED):
------------------------------

Start RW612 board running on_off_output_zc or start a Zigbee Coordinator and open the network
Start NXP board running on_off_switch_zed or start a Zigbee End Device and join the network

Once Switch (ZED) is discovered, Output (ZC) starts periodical 
On-Off-On-Off-...


Note: to make this application quiet (no activity, just Data Request & End Device Timeout Request):
update at the beginning of modules/zboss/samples/on_off_switch_zed/src/on_off_switch_zed.c:
- SEND_TOOGLE_PERIOD -1:     don't send ON/OFF toogle
- LONG_POLL_INTERVAL 60000U: send Data Request every 60000ms (1 min)
- KEEP_ALIVE_TIMEOUT 180000: send End Device Timeout Request every 180000ms (3 min)


ota_upgrade_nxp usecase (ZED):
------------------------------

Start a Zigbee Coordinator or Router with OTA Server cluster
Start NXP board

Once OTA client (ZED) has joined NWK, it sends Query Next Image Request to OTA server (ZC) and the OTA upgrade is started.
When completed, the ZED switches to the new image on the next auto-reboot of the board.

Supports SB3.1 secure containers: For updating the radio core (NBU) or the application core (MCU) via NXP ROM bootloader.
This means that any image sent over the air needs to be signed in sb3 format with matching keys.

To sign your application core or radio core with the appropriate keys, please use NXP's MCUXpresso Secure Provisioning Tool, "build image" button (https://nxp.com/sec).
MCXW727C profile: Plain signed image running on on-chip flash

- App core (MCU):
  If *.elf is used, start address is automatically detected.

- Radio core (NBU):
  MCXW72 start address: 0x48800000

Note: On ZC side, the OTA Server shall have the following manufacturer - image_type:
- 1037 - 340E: ota_client_zed signed.bin for MCXW72


light_sample usecase (ZC, ZR, ZED):
-----------------------------------

Start RW612 board running light_zc
Start NXP board running bulb
Wait for 10 seconds
Start NXP board running light_control

Once Dimmable Light is discovered, Light Control (ZED) starts periodical
On-Off-On-Off-...
Command is sending with 15 seconds timeout to bulb (ZR)


simple_gw & ias_zone_sensor usecase (ZC, ZR):
---------------------------------------------

Start RW612 board running simple_gw
Start NXP board running izs_device

After a couple Zone Status Change Notifications:
Restart board running izs_device (ZED)
Once IAS Zone device (ZED) is reconnected to Simple Gateway (ZC), starts periodical Zone Status Change Notifications...
Restart RW612 board simpe_gw (ZC)
Once IAS Zone device (ZED) is reconnected to Simple Gateway (ZC), starts periodical Zone Status Change Notifications...


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


Production config:
------------------

Production config is splitted in 3 sections that is read by Zboss stack unsing nvmem APIs,
refer to structure hw_param in modules/zboss/platform/osif/zb_osif_prod_cfg.c

The 3 sections are ieee802154_eui64, zb_settings & zb_secured:

    +-----------+------------------------------------------------+
    | Size      | Description                                    |
    +-----------+------------------------------------------------+
    | ieee802154_eui64:                                          |
    |   8 bytes | IEEE 802.15.4 MAC Address                      |
    +-----------+------------------------------------------------+
    | zb_settings:                                               |
    |   8 bytes | header (crc: 32bits, len: 16bits, ver: 16bits) |
    |  40 bytes | Channel masks (10 x 4-byte masks)              |
    | 270 bytes | TX power values (one per channel)              |
    |   1 byte  | Options byte                                   |
    +-----------+------------------------------------------------+
    | zb_secured:                                                |
    |  18 bytes | Install code                                   |
    |   4 bytes | Passcode                                       |
    +-----------+------------------------------------------------+

All these parameters are used by the stack in a structure zb_production_config_ver_4_t
(refer to modules/zboss/include/zboss_api.h), where hdr.crc is calculated on the rest
of the data structure.


Storage location:

    +-----------+-----------------------+-----------------------+-----------------------+
    | Target    | ieee802154_eui64 addr | zb_settings addr      | zb_secured addr       |
    +-----------+-----------------------+-----------------------+-----------------------+
    | MCXW72    | IFR0: 0x2002000       | IFR0: 0x2002010       | IFR0: 0x2002200       |
    +-----------+-----------------------+-----------------------+-----------------------+

Example config:

    IEEE Address:   11 22 33 44 55 66 77 88
    Channel Mask:   00 00 08 00
    TX Power:       7 dBm (all 270 values)
    Options:        0x00
    Install Code:   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
                    ^ If an install code is defined, a CRC-16-CCITT (poly 0x1021,
                      init 0xFFFF, reflected 0x8408, XorOut 0xFFFF) is computed
                      over the raw code bytes and appended little-endian.
    Passcode:       00 00 00 00
    Header.Version: 0x0004
    Header.Length:  349 bytes (0x015D)
    Header.CRC:     0x72CB7E72
                    ^ CRC-32 (zlib/ISO 3309) over the 345-byte config body:
                      len(2) + version(2) + channel_masks(40) + ieee_addr(8) +
                      tx_power(270) + options(1) + install_code(18) + passcode(4).
                      Stored little-endian at offset 0 of zb_settings.


Example commands for MCXW72:

    # Pre-config: Erase IFR0 sector
    blhost -p /dev/ttyACMx -- flash-erase-region 0x2002000 8192

    # Step 1: Write ieee802154_eui64 at 0x2002000
    blhost -p /dev/ttyACMx -- write-memory 0x2002000 "{{11 22 33 44 55 66 77 88 00 00 00 00 00 00 00 00}}"

    # Step 2: Write zb_settings at 0x2002010
    blhost -p /dev/ttyACMx -- write-memory 0x2002010 "{{72 7E CB 72 5D 01 04 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00}}"

    # Step 3: Write zb_secured at 0x2002200
    blhost -p /dev/ttyACMx -- write-memory 0x2002200 "{{00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00}}"

    # Post config: Reset device
    blhost -p /dev/ttyACMx -- reset

On next restart, you should have: "signal 23: ZDO PRODUCTION_CONFIG_READY, status 0" in your app:
- read production config is ok
- production config header has a valid version, len and CRC


Provisioning the production config:
-----------------------------------

Two helpers are provided to write the plaintext production config into IFR0.
Both compute the header CRC-32 and the install-code CRC-16 automatically, so the
config values are given as plaintext and no CRC is entered by hand.

1. Host tool (blhost command generator) - build_commands.py

   Reads config.txt (in the same folder) and generates the blhost write-memory
   commands (written to resulted_commands.txt). Edit config.txt with your values:

       IEEE Address   : 11 22 33 44 55 66 77 88
       Channel Mask   : 00 08 00 00        # little-endian; 00 08 00 00 = channel 11
       TX Power       : 7 dBm
       Options        : 0x43               # low 2 bits = IC type (set automatically
                                           # from the code length); set bit 6 (0x40)
                                           # when a passcode is provided. IC+passcode
                                           # 128-bit -> 0x43
       Install Code   : 01 02 20 33 30 44 01 02 20 33 30 44 01 02 20 33  # 6/8/12/16 raw code bytes
       Passcode       : 01 02 03 04
       Header.Version : 0x0004

   The IC type (options low 2 bits) is derived from the install-code length
   (6/8/12/16 bytes -> 48/64/96/128-bit) and the CRC-16 is appended for you.
   Set TARGET / PORT at the top of the script, then:

       python3 build_commands.py
       # run the generated blhost commands (resulted_commands.txt) against the board

2. On-target Zephyr app - prodcfg_provision

   Standalone one-shot Zephyr firmware app that writes the same plaintext config
   into IFR0 at runtime via the flash API (no blhost, no host connection needed).
   Edit the values in the "USER CONFIGURATION" section at the top of src/main.c
   (ieee_addr, CHANNEL, TXPOWER_DBM, options, install_code, passcode), then build
   and flash it as a normal Zephyr sample:

       west build -b frdm_mcxw72 modules/zboss/samples/prodcfg_provision \
            -d _build/frdm_mcxw72/prodcfg_provision -p

   Flash and run it once (see the "OK: IFR0 provisioned ..." console message),
   then reset the board and load your Zigbee firmware. See the app's
   readme-prodcfg_provision.txt for details.



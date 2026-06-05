+-----------------------------------------------------------------------------+
|                        ZBOSS IW61x delivery package                         |
+-----------------------------------------------------------------------------+


 Copyright 2023-2025 NXP

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

Date:    Tue, 12 May 2026 11:52:52 +0000
Version: 019.2602.049
Sha1:    2929807
Zboss:   zoi_release-4.2.2.0


Delivery content:
=================

output
├── bin                          => Host binaries compiled for i.MX
│   ├── examples
│   │   ...                      => Pre-build examples
│   └── zb_mux                   => MUX daemon
├── changeLog.txt
├── docs                         => Docs from zboss
│   ├── readme-example-*.txt     => Description of examples
│   ├── test-example-*.txt       => Description of examples test steps
│   └── zboss-api-user-guide
│       └── index.html           => Zboss Api documentation (html format)
├── examples                     => Examples sources code & Makefile
│   ├── cli_nxp                  => Example of Command Line Interface
│   ├── custom_cluster           => Example of custom cluster
│   ├── distributed              => Example of distributed Network
│   ├── dualpan_nxp              => Example of dualpan config management
│   ├── gp_proxy_zr              => Example of Green Power Proxy router
│   ├── ias_zone_sensor          => Example of Intruder Alarm System zone sensor device
│   ├── level_control_sample     => Example of ZCL Level Control API Usage
│   ├── light_sample             => Example of distributed Network
│   ├── manuf_specific_cmds_and_attrs
│   │                            => Example of Manufacturer specific commands and attributes implementation
│   ├── multi_ep                 => Example of Multiple Endpoints support
│   ├── onoff_server             => Example of OnOff cluster
│   ├── ota_upgrade_nxp          => Example of OtaUpgrade cluster using NXP Manufacturer Code & Image Type
│   ├── r23_new_api              => Example of R23 new API sample
│   ├── simple_gw                => Example of Coordinator
│   ├── scenes                   => Example of Scenes cluster
│   ├── tc_swap_out              => Example of Trust Center swap out sample
│   ├── thermostat               => Example of Thermostat
│   ├── zb-ctl                   => Example of Command Line Interface client controlling zb_daemon
│   └── zb_daemon                => Example of Command Line Interface run in background and controlled by zb-ctl
├── include
│   ...                          => Zboss headers files for template or custom Zigbee app
├── libs
│   ...                          => Zboss libraries files for template or custom Zigbee app
│   └── zb_vendor.h              => Zboss config header files for template or custom Zigbee app
├── README.txt                   => This document
├── scripts
│   └── imx-dualpan.sh           => Script to start ZibBee-OpenThread application(s) on IMX-IW612 manually
├── services                     => Services to start a ZibBee application on i.MX-IW61x automatically
│   ├── etc
│   │   └── default
│   │       └── zb_mux_app.config
│   ├── usr
│   |   └──lib
│   │      └── systemd
│   │          └── system
│   │              ├── zb_app.service
│   │              ├── zb_config.service
│   │              └── zb_mux.service
│   └── usr
│       └── sbin
│           ├── zb_app.sh
│           ├── zb_config.sh
│           └── zb_mux.sh
└── template
    ├── Makefile_template        => Template Makefile
    └── Options                  => Makefile options


Dualpan overview:
=================

Thread and Zigbee applications are running on top for zb_mux daemon.
zb_mux daemon is running on top of SPI to exchange Spinel message to the IW612.

         +------------+                +------------+
         | Thread app |                | Zigbee app |
         +-----+------+                +-----+------+
               |                             |
  Virtual UART |                             | Virtual UART
               |        +------------+       |
               +--------+   zb_mux   +-------+
                        +-----++-----+
                              ||
                              || SPI Bus
                              ||
                        +-----++-----+
                        |   IW612    |
                        +------------+


Overview:
=========

- IMX Configuration
- Run Zigbee application automatically
	- zb_config service
	- zb_mux service
	- zb_app service
	- run & check services
- Run Zigbee application manually
	- Debugging options of ./imx-dualpan.sh
	- onoff_server usecase (ZC, ZED)
	- ota_upgrade_nxp usecase (ZC, ZR)
	- light_sample usecase (ZC, ZR, ZED)
	- simple_gw usecase (ZC)
	- scenes usecase (ZC, ZED)
	- thermostat usecase (ZC, ZR)
	- tc_swap_out usecase (ZC, ZED)
	- custom_cluster usecase (ZC, ZR)
	- dualpan_nxp usecase (ZC, ZR, ZED)
	- level_control_sample usecase (ZC, ZED)
	- manuf_specific_cmds_and_attrs usecase (ZC, ZED)
	- multi_ep usecase (ZC, ZED)
	- r23_new_api usecase (ZC, ZR, ZED)
	- ias_zone_sensor usecase (ZC, ZR)
	- cli_nxp usecase (ZC/ZR, ZED)
- Run thread
	- Configure thread role
- Run zigbee and thread (dualPAN)
- Run both thread & zigbee
- Zigbee examples
	- BSP / Toolchain setup
	- Environment setup
	- Compilation
	- Deployment & execution
- Tools setup
	- Wireshark
- Misc topics
	- Tx power config
	- Manufacturer code
	- Re-open zigbee network
	- Concentrator mode
	- Memory buffer handling

IMX Configuration:
==================

IMX board should be flashed with an image supporting:
- IW612
- Openthread

For SPI interrupt, zb_mux daemon is working the same way as openthread-spi


zb_mux daemon can dump debug information using env variable:
- SPI traces:
  export DUMP_SPI=0
  => 0: no log
  => 1: dump SPI header
  => 2: dump SPI data
  => 3: both
- Spinel traces:
  export DUMP_SPINEL=2
  => 0: no log
  => 1: dump SPINEL Raw data
  => 2: dump SPINEL interpreted data (for Zigbee)
  => 3: both


Run Zigbee application automatically:
=====================================

To start automatically zigbee application, 3 systemd services are provided:
- zb_config: start ZBOSS Configuration
- zb_mux:    start ZBOSS Muxer
- zb_app:    start ZBOSS Application

zb_config is composed of 2 files:
/usr/lib/systemd/system/zb_config.service: defines configuration of the service
/usr/sbin/zb_config.sh:                   script creating env file /tmp/zb_environment_vars based on zb_mux_app.config

zb_mux & zb_app is composed of 3 files:
/etc/default/zb_<mux|app>.env:            defines environment of the service
/usr/lib/systemd/system/zb_<mux|app>.service: defines configuration of the service
/usr/sbin/zb_<mux|app>.sh:                script running the service

It requires to have:
/usr/sbin/zb_mux
/usr/bin/<zboss application name>

=> Copy all these files from delivery package to the target:
set IMX_IPADDR to the ip address of the target
set ZB_APP to the zboss application name to test
scp    <...>/bin/zb_mux             root@${IMX_IPADDR}:/usr/sbin/.
scp    <...>/bin/examples/${ZB_APP} root@${IMX_IPADDR}:/usr/bin/.
scp -r <...>/services/*             root@${IMX_IPADDR}:/
=> Update on the target the zboss application name to test
ssh root@${IMX_IPADDR} vi /etc/default/zb_app.env
  Comment:   ZB_APP_NAME=simple_gw
  Uncomment: ZB_APP_NAME=<zboss application name>
=> Update correct firmware in /lib/firmware/nxp:
  sduart_nw61x_v1_zb_dual_pan.bin.se: iw612 WiFi|Combo firmware
  uartspi_n61x_v1_zb_dual_pan.bin.se: iw612 Bt firmware
  sduartspi_iw610.bin.se:             iw610 WiFi|Combo firmware
  uartspi_iw610.bin.se:               iw610 Bt firmware


zb_config service:
------------------

This service creates an environment file required by mux and app: /tmp/zb_environment_vars
It also creates /var/local/zboss working directory.

/tmp/zb_environment_vars will be the configuration file used to run the service, which is based on zb_mux_app.config.
Using the generated file allows you to perform configuration testing without modifying zb_mux_app.config.
Please note that /tmp/zb_environment_vars is generated every time the service is started.
Once the configuration is validated and the user wants to make it persistent, zb_mux_app.config should be updated accordingly.

zb_mux service:
---------------

This service requires to have zb_config service started.

Its working directory is /var/local/zboss that contains:
 - zb_mux.log (configured by mux_trace)
 - zb_mux.console (configured by ZB_MUX_OUT, may also contains debug information)

It will start zb_mux based on config created by config service.
It can load btnxpuart driver(load fimware in IW61x if needed)

zb_app service:
---------------

This service requires to have zb_mux service started.

Its working directory is /var/local/zboss that contains:
 - <zboss application name>.log (if mux_trace is not 0:0)
 - <zboss application name>.console (configured by ZB_APP_OUT, may also contains debug information)
 - <zboss application name>.nvram (factory config)

It will clean working directory, perform factory reset if required and start application.

run & check services:
--------------------

in case a service bas changed:
  systemctl daemon-reload

manual start:
  systemctl start zb_config.service
  systemctl start zb_mux.service
  systemctl start zb_app.service
  systemctl start otbr-agent.service

manual stop:
  systemctl stop otbr-agent.service
  systemctl stop zb_app.service
  systemctl stop zb_mux.service
  systemctl stop zb_config.service

auto start:
  systemctl enable zb_config.service
  systemctl enable zb_mux.service
  systemctl enable zb_app.service
  systemctl enable otbr-agent.service

auto stop:
  systemctl disable otbr-agent.service
  systemctl disable zb_app.service
  systemctl disable zb_mux.service
  systemctl disable zb_config.service

  boot messages:
             Starting ZBOSS Configuration...
    ...
    [  OK  ] Finished ZBOSS Configuration.
    [  OK  ] Started ZBOSS Muxer.
    [  OK  ] Started ZBOSS Application.


check status:
  systemctl status zb_config.service
  systemctl status zb_mux.service
  systemctl status zb_app.service
  systemctl status otbr-agent.service

check journal:
  journalctl -xeu zb_config.service
  journalctl -xeu zb_mux.service
  journalctl -xeu zb_app.service
  journalctl -xeu otbr-agent.service

check runtime:
tail -f /var/local/zboss/zb_mux.<console|log>
tail -f /var/local/zboss/<zboss application name>.<console|log>


Note:
-----

In dual pan context otbr-agent is running over UART (through through /dev/ttyopenthread created by zb_mux) while directly over SPI on standalone.
otbr-agent service will automatically detect whether config service is running and start otbr-agent with appropriate command line parameters.


Run Zigbee application manually:
================================

This section is for test purpose, it is one time shot

imx-dualpan.sh script allows to:
- configure SPI interrupt
- download IW612 firmware (optional)
- delete NVRAM data (optional)
- start zb_mux
- start zb application

imx-dualpan.sh script requires to have fw_loader_imx_lnx in $PATH

example with firmware download
./imx-dualpan.sh --ch <channel> --zb <zb_app> [--fw <IW612-firmware>]

example without firmware download
./imx-dualpan.sh --ch <channel> --zb <zb_app>

The application gets the channel number from the env variable MACSPLIT_CHANNEL,
except for zb_cli.
The application gets the virtual uart dev from the env variable MACSPLIT_TTY.

The script will ask if the user wants to do a factory reset:
- if so, .nvram & .log of that app is removed
NVRAM file contains the device address, panid, nwk keys ...

Notes:
------
  zb Log level:
  - only zb_mux takes arguments for log level (-t 4:0xFFFFFFFF)
  - log level of others applications read env variables ZB_TRACE_LEVEL & ZB_TRACE_MASK.
    by default, release build uses ZB_TRACE_LEVEL=4 ZB_TRACE_MASK=0x00000800
    by default, debug   build uses ZB_TRACE_LEVEL=4 ZB_TRACE_MASK=0xffffffff
    details:
    COMMON      0x00000001U  /**< Common subsystem. */
    MEM         0x00000002U  /**< MEM subsystem (buffers pool). */
    MAC         0x00000004U  /**< MAC subsystem. */
    NWK         0x00000008U  /**< NWK subsystem. */
    APS         0x00000010U  /**< APS subsystem. */
    ZSE         0x00000020U  /**< ZSE subsystem. */
    ZDO         0x00000040U  /**< ZDO subsystem. */
    SECUR       0x00000080U  /**< Security subsystem. */
    ZCL         0x00000100U  /**< ZCL subsystem. */
    ZLL         0x00000200U  /**< ZLL/Touchlink subsystem. */
    SSL         0x00000400U  /**< SSL subsystem - not really used */
    APP         0x00000800U  /**< User Application */
    LWIP        0x00001000U  /**< LWIP is used, else free */
    ALIEN       0x00002000U  /**< Some special debug */
    MAC_API     0x00008000U  /**< MAC API subsystem */
    MACLL       0x00010000U  /**< MAC LL subsystem */
    SPECIAL1    0x00020000U  /**< Special subsystem */
    BATTERY     0x00040000U  /**< Battery subsystem */
    OTA         0x00080000U  /**< OTA subsystem */
    TRANSPORT   0x00100000U  /**< Transport subsystem */
    USB         0x00200000U  /**< USB subsystem */
    SPI         0x00400000U  /**< SPI subsystem */
    UART        0x00800000U  /**< UART subsystem */
    PTA         0x01000000U  /**< PTA subsystem */
    JSON        0x01000000U  /**< JSON subsystem */
    HTTP        0x02000000U  /**< HTTP subsystem */
    CLOUD       0x04000000U  /**< Interface to the Cloud */
    ZBDIRECT    0x08000000U  /**< Zigbee Direct subsystem */
    DIAGNOSTIC  0x10000000U  /**< Diagnostic subsystem */
    NS          0x20000000U  /**< Network simulator subsystem */
    TEST        0x40000000U  /**< Subsystem for tests and CI */
    ADDR        0x80000000U  /**< to trace address lock / unlock operations */
  - these settings can be customized in imx-dual-pan.sh script

  ieee address:
  - by default, the application configures it with the API zb_set_long_address() called in MAIN()
  - to be able to configure it at run time, the env variable MACSPLIT_IEEE_ADDR is checked
  - this setting can be customize in imx-dual-pan.sh script, disabled by default

  zb_mux or zb application can be stopped using the signal TERM:
  kill -TERM `pidof <app>`

  Green Power:
  any ZR supports GP Proxy role by BDB specification v3.0.1


Debugging options of ./imx-dualpan.sh:
--------------------------------------

Refer ./imx-dualpan.sh --help for details

Zboss stack is logging into a .log file (zb_mux or zb app name) using zb Log level:
--zberr:                 to track it live
--zblog <debug|release>: configure zboss stack log level

Zigbee mux & applications log high level information into the console by default, to configure it:
zb_mux: --muxout    <0~3>: disable log, enable log on the console and/or zb_mux.console file
        --dbgspi    <0~3>: log SPI headers & frames
        --dbgspinel <0~3>: log SPINEL frames (raw data and/or interpreted) going throught SPI interface
zb_app: --zbout     <0~3>: disable log, enable log on the console and/or <zb_app>.console file
        --dbgtty    <0~3>: log TTY frames (raw data and/or interpreted) going throught TTY interface


onoff_server usecase (ZC, ZED):
-------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb on_off_output_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb on_off_switch_zed [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first on_off_output_zc (ZC)
Hit <Enter> on IMX #2 to start second on_off_switch_zed (ZED)

Once Switch (ZED) is discovered, Output (ZC) starts periodical 
On-Off-On-Off-...


ota_upgrade_nxp usecase (ZC, ZR):
---------------------------------

There is a multi client functionality, with three possible set-ups:
- 2 ZR OTA Clients
- 2 ZED OTA Clients
- 1 ZED and 1 ZR OTA Clients

Run ZC with 2 ZR OTA Clients:
On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb ota_server_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb ota_client_zr [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb ota_client_zr [--fw <IW612-firmware>] --ieee 00:00:00:00:01:00:00:01

Run ZC with 2 ZED OTA Clients:
On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb ota_server_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb ota_client_zed [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb ota_client_zed [--fw <IW612-firmware>] --ieee 00:00:00:00:00:00:00:02

Run ZC with 1 ZR and 1 ZED OTA Clients:
On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb ota_server_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb ota_client_zr [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb ota_client_zed [--fw <IW612-firmware>]

Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first ota_server_zc (ZC)
Hit <Enter> on IMX #2 to start second ota_client_zr|zed (ZR|ZED)
Hit <Enter> on IMX #3 to start second ota_client_zr|zed (ZR|ZED)


light_sample usecase (ZC, ZR, ZED):
-----------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb light_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb bulb [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb light_control [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first light_zc (ZC)
Hit <Enter> on IMX #2 to start second bulb (ZR)

Wait for 10 seconds
Hit <Enter> on IMX #3 to start third light_control (ZED)

Once Dimmable Light is discovered, Light Control (ZED) starts periodical 
On-Off-On-Off-...
Command is sending with 15 seconds timeout to bulb (ZR)


simple_gw usecase (ZC):
-----------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb simple_gw [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first simple_gw (ZC)


scenes usecase (ZC, ZED):
-------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb scenes_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb scenes_zed [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first scenes_zc (ZC)
Hit <Enter> on IMX #2 to start second scenes_zed (ZED)


thermostat usecase (ZC, ZR):
----------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb thermostat_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb thermostat_zr [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first thermostat_zc (ZC)
Hit <Enter> on IMX #2 to start second thermostat_zr (ZR)


tc_swap_out usecase (ZC, ZED):
------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb tcso_zc  [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb tcso_zed [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first tcso_zc (ZC)
Hit <Enter> on IMX #2 to start second tcso_zed (ZED)

Note: after rebooting ZC, and to do the TC swap out, do not perform a factory reset.


custom_cluster usecase (ZC, ZR):
--------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb custom_cluster_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb custom_cluster_zr [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first custom_cluster_zc (ZC)
Hit <Enter> on IMX #2 to start second custom_cluster_zr (ZR)


dualpan_nxp usecase (ZC, ZR, ZED):
----------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zc [--fw <IW612-firmware>] [--ot <ot-apps configured manually on different channel>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zr [--fw <IW612-firmware>] [--ot <ot-apps configured manually on different channel>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zed [--fw <IW612-firmware>] [--ot <ot-apps configured manually on different channel>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first dualpan_zc (ZC)
Hit <Enter> on IMX #2 to start second dualpan_zr (ZR)
Hit <Enter> on IMX #3 to start second dualpan_zed (ZED)


level_control_sample usecase (ZC, ZED):
---------------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb level_control_zc  [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb level_control_zed [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first level_control_zc (ZC)
Hit <Enter> on IMX #2 to start second level_control_zed (ZED)


manuf_specific_cmds_and_attrs usecase (ZC, ZED):
------------------------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb manuf_onoff_output_zc  [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb manuf_onoff_switch_zed [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first manuf_onoff_output_zc (ZC)
Hit <Enter> on IMX #2 to start second manuf_onoff_switch_zed (ZED)


multi_ep usecase (ZC, ZED):
---------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb multiendpoint_zc  [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb multiendpoint_zed [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first multiendpoint_zc (ZC)
Hit <Enter> on IMX #2 to start second multiendpoint_zed (ZED)

r23_new_api usecase (ZC, ZR, ZED):
-----------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb r23_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb r23_zr [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb r23_zed [--fw <IW612-firmware>]

Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first r23_zc (ZC)
Hit <Enter> on IMX #2 to start second r23_zr (ZR)

Wait for 10 seconds
Hit <Enter> on IMX #3 to start third r23_zed (ZED)

ias_zone_sensor usecase (ZC, ZR):
---------------------------------

On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb simple_gw [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb izs_device [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (in case of previous run)

Hit <Enter> on IMX #1 to start first simple_gw (ZC)
Hit <Enter> on IMX #2 to start second izs_device (ZED)

After a couple Zone Status Change Notifications:
Restart IMX #2 izs_device (ZED) WITHOUT factory reset
Once IAS Zone device (ZED) is reconnected to Simple Gateway (ZC), starts periodical Zone Status Change Notifications...
Restart IMX #1 simpe_gw (ZC) WITHOUT factory reset
Once IAS Zone device (ZED) is reconnected to Simple Gateway (ZC), starts periodical Zone Status Change Notifications...

cli_nxp usecase (ZC/ZR, ZED):
-----------------------------

On IMX #1, run: ./imx-dualpan.sh --zb cli_nxp_zczr [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --zb cli_nxp_zed  [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (can also be done using cli command)

Hit <Enter> on IMX #1 to start cli_nxp_zczr (ZC/ZR)
Hit <Enter> on IMX #2 to start cli_nxp_zed (ZED)

Each IMX prompts "zbcli>"

Refer to docs/readme-example-cli_nxp.txt

Note: Dual Pan context:
cli_nxp need to interact with the user to enter cli command. It cannot be run in background.
In case of dualpan, there are 2 applications ot & zb. First appli is run in background.
So to be able to run cli_nxp in dual pan, you must have cli_nxp run in second:
./imx-dualpan.sh --ch <channel> --ot <ot_app> --zb cli_nxp_<zczr|zed> [--fw <IW612-firmware>]


zb_daemon usecase (ZC/ZR, ZED):
-------------------------------

zb_daemon is identical as cli_nxp but can run in background.

On IMX #1, run: ./imx-dualpan.sh --zb zb_daemon_zczr [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --zb zb_daemon_zed  [--fw <IW612-firmware>]
Hit <Enter> after resetting the IW612 (in case of firmware download)
Hit <Enter> to remove the NVRAM file and perform a factory reset (can also be done using cli command)

Hit <Enter> on IMX #1 to start zb_daemon_zczr (ZC/ZR)
Hit <Enter> on IMX #2 to start zb_daemon_zed (ZED)

Both applications can run in background and do not interact with the user.
The interaction is managed by zb-ctl (like it is for OpenThread with ot-daemon/ot-ctl)
Each IMX/zb-ctl prompts "zbcli>"

zb-ctl can be used in two possible ways:

Single command:
zb-ctl <cli_nxp_command> <ENTER>

Multiple commands:
zb-ctl <ENTER>

It takes the same input commands and behaves the same way as cli_nxp.
zb-ctl prompt has extra command compared to cli_nxp:

- exit: command to exit zb-ctl application.

When zb-ctl sends "quit" command, zb_daemon is terminated (or prompted to be restarted if run last in imx-dualpan.sh script).
zb-ctl then tries to reconnect to the server. When it is reconnected, zbcli> prompt appears again.

Refer to docs/readme-example-cli_nxp.txt


In case you are running dualpan (OT and ZB), you will have 3 processes:

         +----------------------------------------------+
         | imx-dualpan.sh --zb zb_daemon --ot ot-daemon |
         +-------------------------+--------------+-----+
                                   |              |
                               +---+---+      +---+---+
          => 3 processes       |zb-ctl |      |ot-ctl |
                               +-------+      +-------+


Run thread:
===========

  cd ~/zboss-dual-pan
  ./imx-dualpan.sh --ch <channel> --ot <ot_app_name_for_uart> [--fw <IW612-firmware>]


Configure thread role:
----------------------

  cd ~/zboss-dual-pan
  ./imx-dualpan.sh --ch <channel> --ot <ot_app_name_for_uart> --otcfg <router|reed|fed|med|sed> --otnet <create|join> [--fw <IW612-firmware>]

Note: using --otcfg option requires to have use as well --otnet to define if we create or join an existing network


Run both thread & zigbee:
=========================

  cd ~/zboss-dual-pan
  ./imx-dualpan.sh --ch <channel> --zb <zb_app_name> --ot <ot_app_name_for_uart> [--fw <IW612-firmware>]
  => Start first Zigbee then Thread

  cd ~/zboss-dual-pan
  ./imx-dualpan.sh --ch <channel> --ot <ot_app_name_for_uart> --zb ./compiled/<zb_app_name> -[--fw <IW612-firmware>]
  => Start first Thread then Zigbee


Zigbee examples:
================

The examples folder provides source examples of Zigbee Coordinator, Router and End device.


BSP / Toolchain setup:
----------------------

First, the IMX BSP need to be installed. It contains the compiler and also the 
sysroot of the target.
to install it:

  chmod +x fsl-imx-xwayland-glibc-x86_64-imx-<XXX>.sh
  sudo ./fsl-imx-xwayland-glibc-x86_64-imx-<XXX>.sh

Example for BSP 6.1.36:

  chmod +x fsl-imx-xwayland-glibc-x86_64-imx-image-multimedia-armv8a-imx93evk-iw612-matter-toolchain-6.1-mickledore.sh
  sudo ./fsl-imx-xwayland-glibc-x86_64-imx-image-multimedia-armv8a-imx93evk-iw612-matter-toolchain-6.1-mickledore.sh

By default, it will install the BSP in /opt/fsl-imx-xwayland/<BSP VERSION>


Environment setup:
------------------

To be able to cross compile for IMX, setup its environment:

. /opt/fsl-imx-xwayland/<BSP VERSION>/environment-setup-<XXX>

To avoid compilation warning and to choose the optimization level, CC commands
needs to be updated.

Example for BSP 6.1.36:
  . /opt/fsl-imx-xwayland/6.1-mickledore/environment-setup-armv8a-poky-linux
  export CC=`echo ${CC} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
  export CXX=`echo ${CXX} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
  export CPP=`echo ${CPP} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`

=> The toolchain is ready to cross-compile for IMX.
=> This information is available using the command: make imx/help imx/<BSP version>, use latest version



Compilation:
------------

Go to an example subfolder, then

  make help
  make clean
  make all


Deployment & execution:
-----------------------

To deploy prebuild and compiled binaries:

  IMX_IPADDR=192.168.0.xxx make imx/deploy


To run prebuild binaries on IMX (via ssh or on uart):

  cd ~/zboss-dual-pan
  ./imx-dualpan.sh --ch <channel> --zb ./prebuild/<zb_app_name> [--fw <IW612-firmware>]

To run compiled binaries on IMX (via ssh or on uart):

  cd ~/zboss-dual-pan
  ./imx-dualpan.sh --ch <channel> --zb ./compiled/<zb_app_name> [--fw <IW612-firmware>]


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

Tx power config:
----------------

Some applications provide an example of configuration of the tx power (get or set):
- examples/onoff_server/on_off_output_zc.c
- examples/ota_upgrade_nxp/ota_server_zc.c
- examples/ota_upgrade_nxp/ota_client_zr.c
- examples/ota_upgrade_nxp/ota_client_zed.c
please refer code under ifdef ZB_MAC_CONFIGURABLE_TX_POWER.

If the environment variable ZB_TX_POWER is define, it will do after started a set with its value,
else a get.

To test it:
  export ZB_TX_POWER=<value 0~255>
  ./imx-dualpan.sh --ch <channel> --zb ./compiled/on_off_output_zc [--fw <IW612-firmware>]

ZB_TX_POWER is in dBm, range [-20, 22], others values will return an error:
tx_power_cb() response ERROR inval param3  channel 21, page 0, power <error value>


Channel management:
-------------------

- Channel config between thread & zigbee:
  At config time, PAN channel can be set for both OT and ZB devices.
  After setting PAN channel for either one, it is stored in the firmware.
  Either ZB or OT apps can have access to the stored PAN channel and then configure it before opening or joining a NWK.

- Zigbee at run time: by default, the channel can be changed due to the following reasons:
  With user intervention:
  - If Mgmt_NWK_Update_req is sent from the application.
  Without user intervention:
  - If the Coordinator discovers a problem with link.
  - If the Coordinator receives Mgmt_NWK_Unsolicited_Enhanced_Update_notify from other device.
  In the latter case, it is possible to disable it using ZBOSS API zb_zdo_disable_network_mgmt_channel_update().

- Thread at run time: TBD


Manufacturer code:
------------------

Default manufaturer code in node descriptor is 0x1037 (NXP).
It can be updated using the API zb_set_node_descriptor_manufacturer_code_req() in
zboss_signal_handler() case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY.


Re-open zigbee network:
-----------------------

An example of re-opening the zigbee network is available in simple_gw under ZB_EXAMPLE_REOPEN_ZIGBEE_NETWORK.
Simple_gw also configure max-children to 64, refer to SIMPLE_GW_DEV_NUMBER.


Concentrator mode:
------------------

An example of concentrator mode is available in simple_gw under ZB_CONCENTRATOR_DISC_TIME.
export ZB_CONCENTRATOR_DISC_TIME=<value 0~4294967295>
To activate concentrator mode without periodical many to one route request, set ZB_CONCENTRATOR_DISC_TIME to 0.


Memory buffer handling:
-----------------------

An endpoint callback can be registered during endpoint creation which is used to handle incoming packets.
If one is registered, then there is 2 possiblities:
-return FALSE: It means stack will handle command and process it on its side.
-return TRUE: It means that stack will not do anything and it's up to the app to handle command.

CAUTION: In case TRUE is returned, any allocated buffer used to process and to handle commands must be freed after used, if not, it leads to buf memory leak.


Firmware crash & recovery:
--------------------------

In case of a firmware crash, it is managed by btnxpuart.ko on Linux host (who manages the firmware download):
- btnxpuart Linux driver will detect the crash of the firmware
- btnxpuart Linux driver will perform a firmware dump:
	Kernel traces: Bluetooth: hci0: ==== Start FW dump ===
- btnxpuart Linux driver will re-download the firmware:
	Kernel traces: Bluetooth: hci0: FW Download Complete: <xxx> bytes

The state of the firmware is available in the file /run/udev/data/btnxpuart:serial0-0.state (FW_READY, ...).

The Linux host zb_mux monitors this state and will block SPI transaction if the state is not FW_READY.

The zboss stack will detect it by the absence of mac-split ack and will raise an error (trace on application's console):
	error Major -2562: MACSPLIT RADIO_HANG_NO_ACK
	/!\ RESET: restart application
And the application auto-restarts.

This can be simulated by doing:
echo 1 > /sys/class/bluetooth/hci0/device/coredump


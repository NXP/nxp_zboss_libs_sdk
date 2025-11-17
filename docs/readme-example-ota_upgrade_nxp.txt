    ZBOSS Zigbee software protocol stack

    Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
    www.dsr-zboss.com
    www.dsr-corporation.com
    All rights reserved.

    Copyright 2024 NXP

    This is unpublished proprietary source code of DSR Corporation
    The copyright notice does not evidence any actual or intended
    publication of such source code.

    ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
    Corporation

    Commercial Usage
    Licensees holding valid DSR Commercial licenses may use
    this file in accordance with the DSR Commercial License
    Agreement provided with the Software or, alternatively, in accordance
    with the terms contained in a written agreement between you and
    DSR.


OTA Upgrade Cluster
====================

This set of applications demonstrates OTA Upgrade Cluster usage.
The set contains three applications:

  - Zigbee Coordinator (OTA Upgrade Server)
  - Zigbee Router (OTA Upgrade Scenes client)
  - Zigbee End Device (OTA Upgrade Scenes client)

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, the devices work on the 0 page 21 channel.

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

The application set structure
------------------------------

  - ota_client.h - *Zigbee Router (OTA Upgrade Scenes client) header file*
  - ota_server.h - *Zigbee Coordinator (OTA Upgrade Server) header file*
  - ota_nxp_definitions.h - *Definitions of file formats*
  - ota_server_zc.c - *Zigbee Coordinator (OTA Upgrade Server) application*
  - ota_client_zr.c - *Zigbee Router (OTA Upgrade Scenes client) application*
  - ota_client_zed.c - *Zigbee End Device (OTA Upgrade Scenes client) application*
  - readme.txt - *This file*

Zigbee Coordinator (OTA Upgrade Server) application
---------------------------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (s)
 - OTA Upgrade (s)

ota_server_zc reads the folder ZBOSS_OTA_SERVER_DIR from env variable (./ota-server-files if not defined) and add up to 10 files.
This folder is prepared by the script create-ota-server-files.sh.
Update on it the following fields:
- zboss_host_tag: with the host version you are using
- zb_folder:      where are imx-dual.sh & zb_mux files
- zb_apps_folder: update the subfolder where are the prebuild apps
- zb_apps_files:  update the list of apps
- iw612_version:  with the firmware version you are using
- fw_folder:      where the firmware files are
- fw_files:       update the list of firmwares

Zigbee Router (OTA Upgrade Scenes client) application
-----------------------------------------------------

Zigbee Router includes following ZCL clusters:

 - Basic (s)
 - OTA Upgrade (c)

ota_client_zr reads the file ./ota-client.cfg to configure with file will be downloaded by OTA.
The downloaded file will be stored in ./OTA-FILE-<date>-<time>-<name>

Zigbee End Device (OTA Upgrade Scenes client) application
---------------------------------------------------------

Zigbee End Device includes following ZCL clusters:

 - Basic (s)
 - OTA Upgrade (c)

ota_client_zed reads the file ./ota-client.cfg to configure with file will be downloaded by OTA.
The downloaded file will be stored in ./OTA-FILE-<date>-<time>-<name>

Applications behavior
----------------------

- The ZC create Zigbee network on the specified channel.
- The ZR/ZED connects to the network.
- The ZR/ZED initiates the process of searching the OTA upgrade server, by issuing a match descriptor request.
- The ZC sends OTA Image Notify command (20 sec after startup - it is scheduled in application), which informs the ZR/ZED about file with manufacturer ID and Image Type ID = ZR/ZED value.
- The ZR/ZED sends OTA Query Next Image request to the ZC with its manufacturer ID, Image Type ID, and current firmware version.
- The ZC sends OTA Query Next Image response with a status code of SUCCESS.
- The ZR/ZED starts the download process by issuing an OTA Image Block Request (Manufacturer ID, Image Type ID, firmware version from previous request. File offset = 0)  The maximum data size shall be set by the client
- The ZC sends OTA Image Block Response.
- The ZR/ZED sends OTA Upgrade End Request command.
- The ZC sends OTA Upgrade End Response command.


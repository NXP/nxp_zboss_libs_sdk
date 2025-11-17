    ZBOSS Zigbee software protocol stack

    Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
    www.dsr-zboss.com
    www.dsr-corporation.com
    All rights reserved.

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


ZBOSS Simple Gateway application
================================

This application is a Zigbee Coordinator device which is implemented using BDB API.
Default operational channel is set by SIMPLE_GW_CHANNEL_MASK.
Maximum operational devices number is 20.

Simple Gateway application includes following ZCL clusters:

  - On/Off Switch Config (s)
  - Identify (s/c)
  - Basic (s)
  - On/Off (c)
  - Scenes (c)
  - Groups (c)
  - IAS Zone (c)


The application folder structure
--------------------------------

- Makefile
- ias_cie_addon.c - *Additions for work with IAS Zone device (with IAS Zone (s) cluster)*
- ias_cie_addon.h - *Additions for work with IAS Zone device (with IAS Zone (s) cluster)*
- readme.txt - *This file*
- simple_gw.c - *Simple Gateway application*
- simple_gw.h - *Simple Gateway application header file*
- simple_gw_device.h - *Simple Gateway application header file, which defines Simple Gateway used clusters*


Application behavior
---------------------

Upload a Simple Gateway binary to your Zigbee device. After powering it on the first time,
a Simple GW forms a ZB network. After power restarting the device, existing ZB network
parameters are used, the same network exists (if NVRAM erasing at start is not configured).
On start up the Simple Gateway opens the network for 180 sec. During this time any device joining
is allowed. In order to join your ZB device (bulb/smart plug/etc), power it on during this time
frame - the device will associate the network automatically.

After the successful association, the Simple GW applications starts discovering On/Off Server
cluster. If it is found, Simple GW configures bindings and reporting for the device and
then then it starts to send On/Off Toggle commands periodically. Timeout between
consecutive commands sending is calculated using the following formula:
T = (device_index * 5) sec.
device_index is a number of joined device: 1, 2, ..., 20.

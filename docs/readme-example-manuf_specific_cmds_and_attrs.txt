    ZBOSS Zigbee software protocol stack

    Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA.
    www.dsr-zboss.com
    www.dsr-corporation.com
    All rights reserved.

    Copyright 2023-2024 NXP

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



ZBOSS Manufacturer specific commands and attributes
==========================

This set of applications demonstrates user defined Custom Cluster implementation.
Also this set demonstrates manufacturer specific commands and attributes implementation. (it is used to turn on/off led)
The set contains two applications:

  - Zigbee Coordinator (which acts as On/Off output)
  - Zigbee End Device (which acts as On/Off switch)

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, both devices work on the 0 page 21 channel.
Devices can be compiled with installcode check(for SE build). By default it is disabled.
In case of compiling these devices for HW, they can be compiled with enabled buttons and leds or with disabled ones, depending on the vendor header used. By default leds and buttons are usually disabled.

The application set structure
------------------------------

 - Makefile
 - manuf_onoff_output_zc.c - *On/Off output coordinator application*
 - manuf_onoff_switch_zed.c - *On/Off switch end device application*
 - readme.txt - *This file*
 - runng.sh - *Script for running setup on Network Simulator*
 - open-pcap.sh - *Script for opening .pcap files*


Zigbee Coordinator (On/Off output) application
-----------------------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (s)
 - Identify (s)
 - OnOff (s)
 - Scenes (s)
 - Groups (s)

Zigbee End Device (On/Off switch) application
----------------------------------------------

Zigbee End Device includes following ZCL clusters:

 - Basic (s)
 - Identify (s/c)
 - OnOff Switch Config (s)
 - OnOff (c)
 - Scenes (c)
 - Groups (c)

This application also includes an example of the user defined ZBOSS main loop.


Applications behavior
---------------------

- Zigbee Coordinator creates network on the 21 channel
- Zigbee End Device joins Zigbee Coordinator using the BDB commissioning
  - Zigbee End Device sends Simple Descriptor Request and Zigbee Coordinator replies with the Simple Descriptor Response
  - Zigbee Coordinator saves the endpoint and the short address of the connected device in 'finding_binding_cb'
- Zigbee End Device sends Permit Join request with permit duration 0 to close the network
- Zigbee End Device starts sending requests of different descriptors to the Zigbee Coordinator (just as an example of the descriptors API usage):
  - Node Descriptor request
  - Power Descriptor request
  - Simple Descriptor request
  - Active Endpoint request
  - System Server Discovery request
- Zigbee End Device (On/Off switch) sequentially sends to coordinator this sequence of command every 3 seconds:
  - Configure reporting request
  - Read reporting configuration request
  - Discover attributes request
  - Discover commands received request
  - Discover commands generated request (Wireshark parses it as an unknown command with code 0x13)
  - ZCL On/Off Toggle command
  - Manufacturer-specific command with id = 0x02 and data 0x01 (1 byte) (Wireshark parses it as an unknown command) (led should turn off)
  - Read attributes
  - Write attributes
  - Manufacturer-specific command with id = 0x02 and data 0x00 (1 byte) (Wireshark parses it as an unknown command) (led should turn on)

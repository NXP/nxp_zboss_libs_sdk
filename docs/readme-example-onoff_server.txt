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



ZBOSS OnOff Cluster set
==========================

This set of applications demonstrates user defined Custom Cluster implementation.
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
 - on_off_output_zc.c - *On/Off output coordinator application*
 - on_off_switch_zed.c - *On/Off switch end device application*
 - readme.txt - *This file*


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
  - Zigbee Coordinator sends Simple Descriptor Request and Zigbee Router replies with the Simple Descriptor Response
  - Zigbee Coordinator saves the endpoint and the short address of the connected device in 'finding_binding_cb'
- Zigbee End Device sends Permit Join request with permit duration 0 to close the network
- Zigbee End Device starts sending requests of different descriptors to the Zigbee Coordinator (just as an example of the descriptors API usage):
  - Node Descriptor request
  - Power Descriptor request
  - Simple Descriptor request
  - Active Endpoint request
  - System Server Discovery request
- Zigbee End Device (On/Off switch) sends ZCL On/Off Toggle command to the Zigbee Coordinator (On/Off output) via BDB binding every 7 sec. If application runs on hardware, Toggle command can be triggered by pressing button.

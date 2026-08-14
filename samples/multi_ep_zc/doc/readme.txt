Multiendpoint app readme
=========================

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



ZBOSS Multiendpoint set
==========================

This set of applications demonstrates implementation of two Zigbee devices, supporting several endpoints.
The set contains two applications:

  - Zigbee Coordinator
  - Zigbee End Device

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, both devices work on the 0 page 21 channel.


The application set structure
------------------------------

 - Makefile
 - multiendpoint_zc.c
 - multiendpoint_zc.h
 - multiendpoint_zed.c
 - multiendpoint_zed.h


Zigbee Multiendpoint Coordinator application
---------------------------------------------

Zigbee Coordinator includes following endpoints with ZCL clusters:

Endpoint 21:
 - Basic (c)
 - OnOff (c)
 - LevelControl (c)

Endpoint 22:
 - Basic (c)
 - OnOff (c)
 - LevelControl (c)

Endpoint 23:
 - Basic (с) 
 - Identify (с) 
 - Time (с)
 - OnOff (c)

Endpoint 24:
 - Identify (c)
 - Basic (c)
 - Alarms (c)

Endpoint 25:
 - Basic (c) 
 - On/Off (c) 
 - Level Control (c)

Zigbee Multiendpoint End Device application
----------------------------------------------

Zigbee End Device includes following ZCL clusters:

Endpoint 21:
 - Basic (s)
 - OnOff (s)
 - LevelControl (s)

Endpoint 22:
 - Basic (s)
 - OnOff (s)
 - LevelControl (s)

Endpoint 23:
 - Basic (s) 
 - Identify (s) 
 - Time (s)
 - OnOff (s)

Endpoint 24:
 - Identify (s)
 - Basic (s)
 - Alarms (s)

Endpoint 25:
 - Basic (s) 
 - On/Off (s) 
 - Level Control (s)



Applications behavior
---------------------

- Zigbee Coordinator creates network on the 21 channel
- Zigbee End Device joins Zigbee Coordinator using the BDB commissioning
- Zigbee End Device starts sending different Write/Read commands for different endpoints
- Zigbee End Device performs binding between On/Off cluster on different endpoints
- Zigbee End Device configures reporting for On/Off cluster on different endpoints
- Zigbee End Device sends different ZCL commands

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


ZBOSS Thermostat Cluster Test setup
=====================================

This application setup demonstrates Thermostat Cluster usage.
Setup contains two applications:

  - Zigbee Coordinator (which implements Thermostat cluster client role)
  - Zigbee Router (which implements Thermostat cluster server role)

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, both devices work on the 0 page 21 channel.


Setup structure
----------------

 - Makefile
 - thermostat_zc - *Zigbee Coordinator application with Thermostat Cluster (c)*
   - thermostat_zc.c
   - thermostat_zc.h
 - thermostat_zr - *Zigbee Router application with Thermostat Cluster (s)*
   - thermostat_zr.c
   - thermostat_zr.h
 - readme.txt - *This file*



Zigbee Coordinator (with Thermostat (c)) application
-----------------------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (s)
 - Identify (s/c)
 - Thermostat (c)



Zigbee Router (with Thermostat (s)) application
----------------------------------------------

Zigbee Router includes following ZCL clusters:

 - Basic (s)
 - Identify (s)
 - Scenes (s)
 - Groups (s)
 - Thermostat (s)
 - Fan Control (s)
 - Thermostat UI (s)

Test scenario is defined in this application.


Setup behavior
---------------

- Zigbee Coordinator (with Thermostat (c) cluster) creates network on the 21 channel
- Zigbee Router joins Zigbee Coordinator using the BDB commissioning
- Zigbee Coordinator starts sending Thermostat cluster commands:
  - Add Scene
  - View Scene
  - Remove a Scene
  - Remove all Scenes
  - Store Scene
  - Recall Scene
  - Get Scene Membership

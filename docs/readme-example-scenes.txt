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


ZBOSS Scenes Cluster Test
================================

This set of applications demonstrates Scenes Cluster usage.
The set contains two applications:

  - Zigbee Coordinator (which acts as On/Off output with Scenes server)
  - Zigbee End Device (which acts as device with Scenes client)

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, both devices work on the 0 page 21 channel.


The application set structure
------------------------------

 - Makefile
 - scenes_zc.c - *On/Off output coordinator application*
 - scenes_zed.c - *Scenes client end device application*
 - readme.txt - *This file*
 - test_output_dev.h -  *Coordinator application header file*
 - test_switch_dev.h - *End Device application header file*
 - scenes_test.h - *A header file with common definitions for testing Scenes (s) cluster*


Zigbee Coordinator (On/Off output with Scenes server) application
------------------------------------------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (s)
 - Identify (s)
 - OnOff (s)
 - Scenes (s)
 - Groups (s)

This application also defines and uses its own NVRAM storage (application-specific NVRAM), if NVRAM feature has been enabled in the ZBOSS build.
Scenes commands are processed in the application. See registered by the ZB_ZCL_REGISTER_DEVICE_CB callback their implementation.

Zigbee End Device (On/Off switch) application
----------------------------------------------

Zigbee End Device includes following ZCL clusters:

 - Basic (s)
 - Identify (s/c)
 - OnOff Switch Config (s)
 - OnOff (c)
 - Scenes (c)
 - Groups (c)

Test scenario is defined in this application.


Applications behavior
---------------------

- Zigbee Coordinator creates network on the 21 channel
- Zigbee End Device joins Zigbee Coordinator using the BDB commissioning
- Zigbee End Device starts sending requests of different descriptors to the Zigbee Coordinator (just as an example of the descriptors API usage):
  - Add Scene
  - View Scene
  - Remove a Scene
  - Remove all Scenes
  - Store Scene
  - Recall Scene
  - Get Scene Membership

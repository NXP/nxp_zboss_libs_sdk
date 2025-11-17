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



ZBOSS Level Control sample
==========================

This set of applications demonstrates usage of ZCL API, its behaviour
depending on ZCL backward compatibility various modes
The set contains four applications:

  - Zigbee Coordinator (which acts as Level Control output)
  - 3 Zigbee End Devices (which act as Level Control switch)
    - Level Control switch operating in ZB_ZCL_LEGACY_MODE
    - Level Control switch operating in ZB_ZCL_AUTO_MODE
    - Level Control switch operating in ZB_ZCL_COMPATIBILITY_MODE

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 8 revision specification.
By default, both devices work on the 0 page 21 channel.

The application set structure
------------------------------

 - Makefile
 - level_control_zc.c - *Level Control output coordinator application*
 - level_control_zed_legacy_mode.c - *Level Control switch end device application operating in ZB_ZCL_LEGACY_MODE*
 - level_control_zed_auto_mode.c - *Level Control switch end device application operating in ZB_ZCL_AUTO_MODE*
 - level_control_zed_compatibility_mode.c - *Level Control switch end device application operating in ZB_ZCL_COMPATIBILITY_MODE*
 - readme.txt - *This file*


Zigbee Coordinator (Level Control output) application
-----------------------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (c)
 - Identify (c)
 - OnOff (s)
 - Level Control (s)
 - Identify (s)

Zigbee End Device (Level Control switch) application
----------------------------------------------

Zigbee End Devices include following ZCL clusters:

 - Basic (s)
 - Identify (s)
 - OnOff (c)
 - Level Control (c)
 - Identify (c)

This application also includes an example of the user defined ZBOSS main loop.


Applications behavior
---------------------

- Zigbee Coordinator creates network on the 21 channel
- Zigbee End Device (level_control_zed_legacy_mode, level_control_zed_auto_mode or level_control_zed_compatibility_mode)
  joins Zigbee Coordinator using the BDB commissioning
- Zigbee End Device (level_control_zed_legacy_mode, level_control_zed_auto_mode or level_control_zed_compatibility_mode)
  sends Simple Descriptor Request and Zigbee Coordinator replies with the Simple Descriptor Response

- Further behaviour depends on which Zigbee End Device is operating as Level Control switch. All of them uses the same api 
  for sending Level Control commands, but the format of them differs based on chosen ZCL compatibility mode
  1. In ZB_ZCL_LEGACY_MODE (default mode):
       If application calls OLD API, then packet will be sent in same format as APPLICATION sent WITHOUT conversion. No any Logic should be applied in case of OLD API.
       But for New API calls we should limit revision of generated command with current attribute value.
  2. In ZCL_AUTO_MODE mode old API calls will be able to upgrade commands to revision set in attribute, New ZCL8 API calls will be the same as in Legacy mode,
       they should sent packet limited to attribute. This mode may be set only manually.
  3. ZCL_COMPATIBILITY_MODE should be enabled automatically after peer_revision_callback will be set. 
     It works as Auto, but with ability to limit for each peer independently. Upgrade also possible - if callback returns higher version than API used, but anyway it should be limited by attribute.

  - level_control_zed_legacy_mode
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move to Level command to level 230 WITHOUT OptionsMask and OptionsOverride fields (pre-ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move command WITHOUT OptionsMask and OptionsOverride fields (pre-ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Stop command WITHOUT OptionsMask and OptionsOverride fields (pre-ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Down direction Step Size 50) WITHOUT OptionsMask and OptionsOverride fields (pre-ZCL8 format) using the old API.
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Up direction Step Size 50) WITHOUT OptionsMask and OptionsOverride fields (pre-ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move to Level command to level 230 WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the new API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Stop command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API. As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Down direction Step Size - 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Up direction Step Size - 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response

  - level_control_zed_auto_mode
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move to Level command to level 230 WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Stop command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Down direction Step Size 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Up direction Step Size 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move to Level command to level 230 WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the new API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Stop command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API. As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Down direction Step Size - 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Up direction Step Size - 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response

  - level_control_zed_compatibility_mode
    - Zigbee End Device reads Cluster Revision attribute and Zigbee Coordinator sends a Response where revision value is 3
      Further the format of sending Level Control commands based on this value (in that case old and new API send commands in ZCL8 format)
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move to Level command to level 230 WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Stop command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Down direction Step Size 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Up direction Step Size 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move to Level command to level 230 WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the new API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Move command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Stop command WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API. As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Down direction Step Size - 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response
    - Zigbee End Device sends Step command (Up direction Step Size - 50) WITH OptionsMask and OptionsOverride fields (ZCL8 format) using the old API.
      As a result Current Value attribute is set to 0xFF
    - Zigbee End Device reads Current Value attribute and Zigbee Coordinator sends a Response

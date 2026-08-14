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



ZBOSS Light Sample
=========================

The demo should be set in the following way:

  1. Flash each device individually and power-off after programming.
  2. Power on Light Coordinator.
  3. Power on Dimmable Light device.
  4. Power on Light Control device.

For the detailed information of the each device, refer to the sections below.

The application set structure
-----------------------------------

  - Makefile
  - dimmable_light
    - bulb.c
    - bulb.h
    - bulb_hal.c
    - bulb_hal.h
    - zb_ha_bulb.h
  - light_control
    - light_control.c
    - light_control.h
    - light_control_hal.c
    - light_control_hal.h
  - light_coordinator
    - light_zc.c
    - light_zc.h
    - light_zc_hal.c
    - light_zc_hal.h
  - readme.txt - *This file*


Light Coordinator application
--------------------------------

Light Coordinator device establishes and joins devices to the Zigbee network.
LED1 indicates the program start.

Dimmable Light application
------------------------------

Dimmable Light joins the ZC and waits for a Light Control to join the network.
Both On/Off and Level Control clusters are supported.
Device state and brightness level are store in Non-Volatile Memory, so bulb's parameters are restored after power cycle.
If HW supports LEDs, level control is obtained by Green LED connected to the PWM channel.
LED1 indicates the program start.
LED2 indicates commissioning status (ON when joined the network).
LED3 indicates the bulb state. Lights when On/Off cluster On command received.

Dimmable Light can be started with ZDD (Zigbee Direct Device) capability.
For this it is needed to use the bulb_zdd application. bulb_zdd will start in an unprovisioned state and will wait
for commands from ZVD.

ZVD application can connect to ZDD, establish secure session and
send a command to join the device to Light Coordinator's network by MAC association method.

Light Control application
---------------------------------

Light Control joins the ZC/ZR and searches any device with On/Off or Level Control cluster.

If HW supports buttons, the device has two buttons:
- the "SW2" button is a ON/UP button,
- the "SW1" button is a OFF/DOWN button.
The device behavior depends on the button's pressed time:
If the button is pressed and released less than 1 second, On (Off) command will be send to the bulb.
If the button is pressed more than 1 second, Step (Up or Down) command will be continuously send to the bulb until the button is released.
If HW supports LEDs, the white LED is used for the user notification, which button is being pressed (LED is on for ON/UP button, off for OFF/DOWN button).

LED1 indicates the program start.
LED2 indicates commissioning status (ON when joined the network).

If the device has two buttons:

  - the "Button 1"  sends On command,
  - the "Button 2"  sends OFF command,

If HW does not support buttons - once Dimmable Light is discovered, Light Control starts periodical On-Off-On-Off-...
command sending with 15 sec timeout.

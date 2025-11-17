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


Dualpan Application
===================

This set of applications demonstrates Dualpan application usage.
The set contains three applications:

  - Zigbee Coordinator
  - Zigbee Router
  - Zigbee End Device

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, the devices work on the 0 page 21 channel.
Above behavior is followed in case MACSPLIT_CHANNEL environment variable is not set via imx-dualpan.sh script [./imx-dualpan.sh --ch 25 ], but if MACSPLIT_CHANNEL is set then it would override the defalut bahavior.

We can run each of ZB application with OpenThread application running along in parallel called DualPan mode, or in Standalone mode:
- DualPan Mode
	1. PAN channel info need to be given as part of argument to imx-dualpan.sh script [--ch <11-26>],
	which internally sets the linux environment variable MACSPLIT_CHANNEL.
	2. If side PAN channel for OpenThread (using ot-ctl application) is NOT already set by creating
	a Thread PAN network, then Zigbee applications(ZC, ZR & ZED) can freely configure the PAN channels
	as mentioned in point #1.
	3. If OpenThread has already created a PAN network, then all Zigbee applications(ZC, ZR & ZED) would
	try configuring the PAN channel from argument to imx-dualpan.sh script, and **FAIL**! But Zigbee
	applications internally sense the OpenThread PAN channel configuration and will RE-CONFIGURE PAN Channel
	to be used by Zigbee applications and proceed further.
	4. This design is in place for having seamless Performance on both Zigbee & OpenThread applications. 

	Run ZC with 1 ZR and 1 ZED dualpan applications with OT daemon:
		On IMX #1, run: ./imx-dualpan.sh --ch <channel> --ot ot-daemon --zb dualpan_zc [--fw <IW612-firmware>]
		On IMX #2, run: ./imx-dualpan.sh --ch <channel> --ot ot-daemon --zb dualpan_zr [--fw <IW612-firmware>]
		On IMX #3, run: ./imx-dualpan.sh --ch <channel> --ot ot-daemon --zb dualpan_zed [--fw <IW612-firmware>]

- Zigbee Standalone Mode
	1. PAN channel info need to be given as part of argument to imx-dualpan.sh script [--ch <11-26>],
	which internally sets the linux environment variable MACSPLIT_CHANNEL.
	2. Here, Zigbee applications(ZC, ZR & ZED) can freely configure the PAN channels
	as mentioned in point #1.

	Run ZC with 1 ZR and 1 ZED dualpan applications in Standalone:
		On IMX #1, run: ./imx-dualpan.sh --ch <channel>  --zb dualpan_zc [--fw <IW612-firmware>]
		On IMX #2, run: ./imx-dualpan.sh --ch <channel>  --zb dualpan_zr [--fw <IW612-firmware>]
		On IMX #3, run: ./imx-dualpan.sh --ch <channel>  --zb dualpan_zed [--fw <IW612-firmware>]

There is a multi client functionality, with three possible set-ups:
- 2 ZR
- 2 ZED
- 1 ZED

Run ZC with 2 ZR:
On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zr [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zr [--fw <IW612-firmware>] --ieee 00:00:00:00:02:00:00:00

Run ZC with 2 ZED:
On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zed [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zed [--fw <IW612-firmware>] --ieee 00:00:00:00:00:00:00:02

Run ZC with 1 ZR and 1 ZED:
On IMX #1, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zc [--fw <IW612-firmware>]
On IMX #2, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zr [--fw <IW612-firmware>]
On IMX #3, run: ./imx-dualpan.sh --ch <channel> --zb dualpan_zed [--fw <IW612-firmware>]

The application set structure
------------------------------

  - dualpan.h - *Zigbee Dualpan header file*
  - dualpan_channel.c - *Zigbee Dualpan <get|set>_pan_channel management file*
  - dualpan_zc.c - *Zigbee Coordinator (Dualpan) application*
  - dualpan_zr.c - *Zigbee Router (Dualpan) application*
  - dualpan_zed.c - *Zigbee End Device (Dualpan) application*
  - readme.txt - *This file*

Zigbee Coordinator application
------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (s)


Zigbee Router application
-------------------------

Zigbee Router includes following ZCL clusters:

 - Basic (s)

Zigbee End Device application
-----------------------------

Zigbee End Device includes following ZCL clusters:

 - Basic (s)

Applications behavior
----------------------

- The ZC/ZR/ZED first request firmware get_pan_channel,
  if it is 0 or identical to zb_channel, it request set_pan_channel
  else it update zb_channel to pan_channel and restart
- The ZC create Zigbee network on the specified channel.
- The ZR/ZED connects to the network.


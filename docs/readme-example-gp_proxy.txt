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


ZBOSS Green Power Proxy (GPP) application
========================================

This application is a simple router for GP device.
ZB PRO functionality with Green Power Basic proxy-basic commissioning.

All ZR devices should support the GP Proxy role by Base Device Behavior specification v3.0.1.

The application set structure
------------------------------

 - Makefile
 - gp_proxy.c - *Proxy router application*


 Application behavior
----------------------
In a basic Green Power Network:

   Device	     gp_proxy		  Sink
   [ GPD ]           [ GPP ]            [ GPS ]
      |---------------->|----------------->|
	  Transmitted	     Forwarded
	  GPD Frames	     GPD Frames

This application acts as a proxy, responsible receiving and forwarding the GPD Frame packets.
The GPP receives frames from the Green Power device, then sends the ZCL Green Power commands
to the Green power sink (GPS).

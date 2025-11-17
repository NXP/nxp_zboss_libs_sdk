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



ZBOSS Custom Cluster set of applications
========================================

This set of applications demonstrates user defined Custom Cluster implementation.
The set contains two applications:

  - Zigbee Coordinator (which implements Custom Cluster Client)
  - Zigbee Router (which implements Custom Cluster Client)

These applications implements Zigbee 3.0 specification, Base Device Behavior specification and Zigbee Cluster Library 7 revision specification.
By default, both devices work on the 0 page 21 channel.

The application set structure
------------------------------

 - Makefile
 - custom_cluster_zc - *Custom Cluster coordinator application*
   - custom_cluster_zc.c
   - custom_cluster_zc.h
 - custom_cluster_zr - *Custom Cluster router application*
   - custom_cluster_zr.c
   - custom_cluster_zr.h
 - readme.txt

Zigbee Coordinator application
-------------------------------

Zigbee Coordinator includes following ZCL clusters:

 - Basic (s)
 - Identify (s/c)
 - Custom (c)

Zigbee Router application
--------------------------

Zigbee Router includes following ZCL clusters:

 - Basic (s)
 - Identify (s)
 - Custom (s)
   - u8:              READ ONLY
   - s16:             WRITE ONLY, changed to READ/WRITE on first write access
   - 24bit:           READ/WRITE
   - 32bitmap:        READ ONLY, write optional
   - ieee:            READ/WRITE
   - char_string:     Internal access, reported as unsupported
   - utc_time:        READ ONLY & Report
   - byte_array:      WRITE ONLY & Report
   - bool:            READ/WRITE & Scene
   - 128_bit_key:     Manuf specific, reported as unsupported
   - long_byte_array: READ/WRITE, unused

Applications behavior
----------------------

- Zigbee Coordinator creates network on ZB_CUSTOM_CHANNEL_MASK channel
- Zigbee Router joins Zigbee Coordinator using the BDB commissioning
  - Zigbee Coordinator sends Simple Descriptor Request and Zigbee Router replies with the Simple Descriptor Response
  - Zigbee Coordinator saves the endpoint and the short address of the connected device in 'finding_binding_cb'
- If the connected device supports the Custom Cluster, Zigbee Coordinator schedules 'send_custom_cluster_cmd'
  to send the Custom Cluster commands.
- Values of the Custom Cluster commands fields are generate randomly
  Command 1 and Command 2 will have the incorrect values with 20% probability
  (can be customized using PROBABILITY_OF_INCORRECT_VALUE)
- Zigbee Router replies with the Custom Cluster responses on Commands 1 and 2 if they have correct parameters,
  otherwise it replies with the Default Response with status code

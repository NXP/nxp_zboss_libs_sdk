Test Script - SDK Samples
=============================
 Objective:
   Test and test scripts under SDK Samples context, performs high-level/functionality checks.

 Devices:
   1. ZC - on_off_output_zc
   2. ZED - on_off_switch_zed

 Initial conditions:
   1. All devices are factory new and powered off until used.

 Test procedure:
   Item 1. Turn ZC on.
   Item 2. Turn ZED on
   Item 3. Turn ZED off
   Item 4. Turn ZED on
   Item 5. Turn ZC off. Wait 1 minute.
   Item 6. Turn ZC on. Wait 1 minute.

 Expected outcome:
    For 'Test procedure' item 1:
      1.1. ZC -> Broadcast: Beacon Request
    For 'Test procedure' item 2:
      2.1. ZC -> Broadcast: Beacon
      2.2. ZED -> ZC: Association
      2.3. ZED -> ZC: BDB commissioning
      2.4. ZED -> ZC: Simple Descriptor Request
      2.5. ZC -> ZED: Simple Descriptor Response
      2.6. ZED -> ZC: Toggle. ZC -> ZED: Default Response. (x5)
    For 'Test procedure' item 4:
      4.1. ZED -> ZC: rejoin
      4.2. ZED -> ZC: Toggle. ZC -> ZED: Default Response. (x5)
    For 'Test procedure' item 6:
      6.1. ZED -> ZC: rejoin
      6.2. ZC -> Broadcast: Parent Announce
      6.3. ZED -> ZC: Toggle. ZC -> ZED: Default Response. (x5)

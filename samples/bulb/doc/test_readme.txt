Test Script - SDK Samples
=============================
 Objective:
   Test and test scripts under SDK Samples context, performs high-level/functionality checks.

 Devices:
    1. ZC - light_coordinator
    2. ZR - bulb
    3. ZED - light_control

Initial conditions:
    1. All devices are factory new and powered off until used.

 Test procedure:
    1. Power on ZC.
    2. Power on ZR.
    3. Turn ZED on.
    4. Turn ZC off.
    5. Turn ZC on.
    6. Turn ZR off.
    7. Turn ZR on.
    8. Turn ZED on/off.

 Expected outcome:
    For 'Test procedure' item 2:
      2.1. ZC -> Broadcast: Beacon request
      2.2. ZC -> Broadcast: Beacon
      2.3. ZR -> ZC: association
      2.4. ZR -> ZC: BDB commissioning
      2.5. ZC -> ZR: Match Descriptor Request for input clusters
      2.6. ZR -> ZC: Match Descriptor Response
    For 'Test procedure' item 3:
      3.1. ZED -> ZC: association
      3.2. ZED -> ZC: BDB commissioning
      3.3. ZC -> ZED: Match Descriptor Request for input clusters
      3.4. ZED -> ZC: Match Descriptor Response
      3.5. ZC -> ZED: Match Descriptor Request for output clusters
      3.6. ZED -> ZC: Match Descriptor Response
      3.7. ZC -> ZED: Bind Request for OnOff cluster
      3.8. ZED -> ZC: Bind Response with Success
      3.9. ZC -> ZED: Bind Request for Level Control cluster
      3.10. ZED -> ZC: Bind Response with Success. ZED power cycle
      3.11. ZED -> ZR: send OnOff commands
    For 'Test procedure' item 4:
      4.1. ZED -> ZR: Rejoin
    For 'Test procedure' item 5:
      5.1. ZC -> Broadcast: Permit Join Request
      5.2. (ZC ->) ZR -> Broadcast: Permit Join Request
      5.3. ZC -> Broadcast: Parent Announce
      5.4. (ZC ->) ZR -> Broadcast: Parent Announce
      5.5. ZR -> ZC: Parent Announce Response
    For 'Test procedure' item 6:
      6.1 ZED -> ZR: send OnOff commands
      6.2. ZED -> ZC: Rejoin
      6.3. ZC -> ZED: Match Descriptor Request for Input clusters
      6.4. ZED -> ZC: Match Descriptor Response
      6.5. ZC -> ZED: Match Descriptor Request for output clusters
      6.6. ZED -> ZC: Match Descriptor Response
    For 'Test procedure' item 7:
      7.1 ZED -> ZR: send OnOff commands
      7.2 Route request ZR -> Broadcast
      7.3 Route reply ZC -> ZR
    For 'Test procedure' item 8:
      8.1. ZED -> ZC: Rejoin
      8.2. ZC -> ZED: Match Descriptor Request for output clusters (use if ZED rejoins to ZC)
      8.3. ZED -> ZC: Match Descriptor Response (use if ZED rejoins to ZC)
      8.4. ZED -> ZR: send OnOff commands

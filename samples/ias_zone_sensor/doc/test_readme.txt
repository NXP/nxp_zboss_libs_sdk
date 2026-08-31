Test Script - SDK Samples
=============================
 Objective:
   Test and test scripts under SDK Samples context, performs high-level/functionality checks.

 Devices:
   1. ZC - simple_gw
   2. ZED - izs_device

 Initial conditions:
   1. All devices are factory new and powered off until used.

 Test procedure:
   1. ZC start.
   2. ZED start.
   3. Turn ZED off
   4. Turn ZED on. Do NOT perform a factory reset.
   5. Turn ZC off.
   6. Turn ZC on. Do NOT perform a factory reset.

 Expected outcome:
   For 'Test procedure' item 2:
     2.1. ZC -> Broadcast: Beacon Request
     2.2. ZC -> Broadcast: Beacon
     2.3. ZED -> ZC: Association
     2.4. ZED -> ZC: BDB commissioning
     2.5. ZED -> ZC: Match Descriptor Request
     2.6. ZC -> ZED: Match Descriptor Response
     2.7. ZC -> ZED: Match Descriptor Request
     2.8. ZED -> ZC: Match Descriptor Response
     2.9. ZC -> ZED: Write Attributes
     2.10. ZED -> ZC: Write Attributes Response
     2.11. ZED -> ZC: Zone Enroll Request
     2.12. ZC -> ZED: Zone Enroll Response
     2.13. ZED -> ZC: Zone Status Change Notification Zone Status: 0x0021
     2.14. ZED -> ZC: Zone Status Change Notification Zone Status: 0x0020
   For 'Test procedure' item 4:
     4.1. ZED -> ZC: Rejoin
     4.5. ZED -> ZC: Zone Status Change Notification Zone Status: 0x0021
     4.6. ZED -> ZC: Zone Status Change Notification Zone Status: 0x0020
   For 'Test procedure' item 6:
     6.1. ZC -> Broadcast: Permit Join Request
     6.2. ZC -> Broadcast: Parent Announce
     6.3. ZED -> ZC: Zone Status Change Notification Zone Status: 0x0021
     6.4. ZED -> ZC: Zone Status Change Notification Zone Status: 0x0020

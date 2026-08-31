Test Script - SDK Samples
=============================
 Objective:
   Test and test scripts under SDK Samples context, performs high-level/functionality checks.

 Devices:
   1. ZC - simple_gw
   2. ZR - bulb

 Initial conditions:
   1. All devices are factory new and powered off until used.

 Test procedure:
   Item 1. ZC start.
   Item 2. ZR start.
   Item 3. Turn ZR off
   Item 4. Turn ZR on
   Item 5. Turn ZC off
   Item 6. Turn ZC on

 Expected outcome:
   For 'Test procedure' item 2:
     2.1. ZC -> Broadcast: Beacon Request
     2.2. ZC -> Broadcast: Beacon
     2.3. ZR -> ZC: Association
     2.4. ZR -> ZC: BDB commissioning
     2.5 ZC -> ZR: Match Descriptor Request
     2.6. ZR -> ZC: Match Descriptor Response
     2.7. ZC -> ZR: Extended Address Request
     2.8. ZR -> ZC: Extended Address Response
     2.9. ZC -> ZR: Bind Request
     2.10. ZR -> ZC: Bind Response
     2.11. ZC -> ZR: Configure Reporting
     2.12. ZR -> ZC: Configure Reporting Response
     2.13. ZC -> ZR: Toggle. ZR -> ZC: Default Response (x2)
     2.14. ZR -> ZC: Report Attribute. ZC -> ZR: Default Response (x2)
   For 'Test procedure' item 4:
     4.1. ZC -> ZR: Toggle. ZR -> ZC: Default Response (x2)
     4.2. ZR -> ZC: Report Attribute. ZC -> ZR: Default Response (x2)
   For 'Test procedure' item 6:
     6.1. ZC -> Broadcast: Permit Join Request
     6.2. (ZC) -> ZR -> Broadcast: Permit Join Request
     6.3. ZC -> ZR: Toggle. ZR -> ZC: Default Response (x2)
     6.4. ZR -> ZC: Report Attribute. ZC -> ZR: Default Response (x2)
     6.5. Check period for Attributes Reporting

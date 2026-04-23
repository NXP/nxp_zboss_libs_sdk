Test Script - SDK Samples
=============================
 Objective:
   Test and test scripts under SDK Samples context, performs high-level/functionality checks.

 Devices:
   1. ZC - multiendpoint_zc
   2. ZED - multiendpoint_zed

 Initial conditions:
   1. All devices are factory new and powered off until used.

 Test procedure:
   Item 1. Turn ZC on.
   Item 2. Turn ZED on.

 Expected outcome:
    For 'Test procedure' item 1:
      1.1. ZC -> Broadcast: Beacon Request
    For 'Test procedure' item 2:
      2.1. ZC -> Broadcast: Beacon
      2.2. ZED -> ZC: Association
      2.3. ZED -> ZC: BDB commissioning
      2.4. ZC -> ZED: Write Attributes: On/Off, state On
      2.5. ZED -> ZC: Write Attributes Response, status Read only
      2.6. ZC -> ZED: Read Attributes: On/Off, 21 EP
      2.7. ZED -> ZC: Read Attributes Response: On/Off, 21 EP, Success
      2.8. ZC -> ZED: Read Attributes: Current Level, 22 EP
      2.9. ZED -> ZC: Read Attributes Response: Current Level, 22 EP, Success
      2.10. ZC -> ZED: Read Attributes: Identify Time, 23 EP
      2.11. ZED -> ZC: Read Attributes Response: Identify Time, 23 EP, Success
      2.12. ZC -> ZED: Read Attributes: Power Source, 24 EP
      2.13. ZED -> ZC: Read Attributes Response: Power Source, 24 EP, Success
      2.14. ZC -> ZED: Bind Request: On/Off Cluster, 22 ZC EP -> 22 ZED EP
      2.15. ZED -> ZC: Bind Response: Success
      2.16. ZC -> ZED: Bind Request: On/Off Cluster, 23 ZC EP -> 23 ZED EP
      2.17. ZED -> ZC: Bind Response: Success
      2.18. ZC -> ZED: Bind Request: On/Off Cluster, 24 ZC EP -> 24 ZED EP
      2.19. ZED -> ZC: Bind Response: Success
      2.20. ZC -> ZED: Bind Request: On/Off Cluster, 25 ZC EP -> 25 ZED EP
      2.21. ZED -> ZC: Bind Response: Success
      2.22. ZC -> ZED: Configure Reporting: On/Off, Reported, 22 ZC EP -> 22 ZED EP
      2.23. ZED -> ZC: Configure Reporting Response: Success
      2.24. ZC -> ZED: Configure Reporting: On/Off, Reported, 23 ZC EP -> 23 ZED EP
      2.25. ZED -> ZC: Configure Reporting Response: Success
      2.26. ZC -> ZED: Configure Reporting: On/Off, Reported, 24 ZC EP -> 24 ZED EP
      2.27. ZED -> ZC: Default Response: Unsupported Cluster
      2.28. ZC -> ZED: Configure Reporting: On/Off, Reported, 25 ZC EP -> 25 ZED EP
      2.29. ZED -> ZC: Configure Reporting Response: Success
      2.30. ZC -> ZED: On, 21 ZC EP -> 21 ZED EP
      2.31. ZED -> ZC: Default Response: Success
      2.32. ZC -> ZED: On, 22 ZC EP -> 22 ZED EP
      2.33. ZED -> ZC: Default Response: Success
      2.34. ZC -> ZED: Off, 23 ZC EP -> 23 ZED EP
      2.35. ZED -> ZC: Default Response: Success
      2.36. ZC -> ZED: Off, 24 ZC EP -> 24 ZED EP
      2.37. ZED -> ZC: Default Response: Unsupported Cluster
      2.38. ZC -> ZED: Off, 24 ZC EP -> Broadcast ZED EP
      2.39. ZED -> ZC: Default Response: Success, 21 EP
      2.40. ZED -> ZC: Default Response: Success, 22 EP
      2.41. ZED -> ZC: Default Response: Success, 23 EP
      2.42. ZED -> ZC: Default Response: Success, 25 EP
      2.43. ZC -> ZED: Off, 23 ZC EP -> 24 ZED EP
      2.44. ZED -> ZC: Default Response: Unsupported Cluster
      2.45. ZC -> ZED: Move to Level, 21 ZC EP -> Broadcast ZED EP
      2.46. ZED -> ZC: Default Response: Success, 21 EP
      2.47. ZED -> ZC: Default Response: Success, 22 EP
      2.48. ZED -> ZC: Default Response: Success, 25 EP

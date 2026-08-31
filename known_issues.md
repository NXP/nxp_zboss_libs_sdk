# Known Limitations
## 2026-Q3 
**Zboss Libs:** 4.2.2.0 <br>

**Zboss Host**: 019.2603.065 <br>

#### Warning:
2026-09-30: hal_nxp SHA updated from 0c8f49a767b205c25e63da24a801bdbeb3e2293b to 9b775f914b4998fb2e3d1f98d735f9307217882e to fix a blob retrieval issue. No Zigbee stack change.

### MCX-W72
> Encrypted datasets are lost during NVRAM page migration <br>
A NVRAM page is 16kB large. When a page is full, dataset are written on 2nd page, and new ELE master key is generated. However, if some encrypted data are still on 1st page, it will be impossible to decrypt them anymore.
Thus, if NVRAM content is smaller that 16kB (1 page), this issue is not observed. Therefore, ZED are not impacted by this limitation.
When operating as ZR, a page change usually occurs when there is more than ~20 siblings/children neighbor devices.

> ZR Sporadic address mismatch on relayed packets <br>
Failure observed on Zigbee Compliance Test tp_pro_bv-08 due to a sporadic address mismatch on ZR side (Node Descriptor Request and Buffer Test Request packets sometimes relayed to the wrong device).

> Clusters with no attributes show Cluster revision attribute on manufacturer specific attribute discoveries <br>

 Copyright 2024-2026 NXP

 NXP Proprietary.
 This software is owned or controlled by NXP and may only be used strictly
 in accordance with the applicable license terms. By expressly accepting
 such terms or by downloading, installing, activating and/or otherwise using
 the software, you are agreeing that you have read, and that you agree to
 comply with and are bound by, such license terms. If you do not agree to be
 bound by the applicable license terms, then you may not retain, install,
 activate or otherwise use the software.


Commands description:
=====================

When the application is running (cli_nxp_zczr, cli_nxp_zed or zb_daemon & zb-ctl), a prompt is displayed:
zbcli> 

When a command is executed, a status is returned:
Ok
Error: <ERROR CODE>

A post command message can occur (if the command returned Ok), like
network_open_cb() OK
network_leave_cb() OK
It is a response to a request.
Note that the same cannot be run twice, we need to wait for the response to a request before re-doing the same command.

Top & down arrows can be used to recall previous commands
Multilines are supported by the console.
Menus can be different on zrzc or zed.

Commands starting by '#' are ignored


Run information:
----------------

When the cli starts, it sends to the user: "zbcli: application has started"
When the cli starts the stack and detects the firmware has booted, it sends to the user: "zbcli: firmware has started"

In case of a firmare crash & recovery (refer to README.txt §Firmware crash & recovery) the cli is auto-restarted.
It can be detected by monitoring "zbcli: application has started"
AND THE CONFIGURATION NEEDS TO BE RE-APPLIED.
To re-<open|join> an existing network, don't erase the nvram:
nvram erase_at_start 0


Basic commands:
---------------

help:                  display basic commands & list of menus available (config, network, ...)
sleep [duration]:      delay in seconds
wait_cb [nb] [timeout]:wait numbers [0-255] callbacks or signals to occur for at most timeout [0-65535] in ms
quit:                  leave the application


help command:
-------------

help:                  display basic commands & list of menus available (config, network, ...)
<basic> help:          display help of basic command
<menu> help:           display menu help: for each command it display its description
<menu> <command> help: display command help


Menu configuration:
-------------------

Flags:
- C: Config time, before starting the stack
- R: Run time, before or after starting the stack
- M: Mandatory parameter
- O: Optional parameter

config help:                            R-  display config commands
config ieee_addr [IEEE]:                CM  configure the IEEE address: [AA:AA:AA:AA:AA:AA:AA:AA]
config role [role]:                     CM  configure the role: [ coordinator router ]
config channel [channel]:               CM  configure the channel number or mask: [11-26|0xMMMMMMMM]
config behavior [behavior]:             CO  configure the stack behavior [r22|r23]
config nwk_distrib [distrib]:           CO  configure network distributed: [0-1]
config nwk_key [idx] [KEY128]:          CO  configure secur network key index: [0-3] key [KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK:KK]
config panid [panid]:                   RO  configure the panid: [0x0~0xFFFF]
config extpanid [extpanid]:             CO  configure the extpanid: [EE:EE:EE:EE:EE:EE:EE:EE]
config max_children [nb]:               CO  configure max devices to join, nb: [ 0-250 ]
config factory_reset [factory_reset]:   CO  configure start with factory reset: [0-1], deprecated
config print:                           C-  printf config values
config start:                           -M  start ZigBee stack
config status:                          R-  status ZigBee stack
config stop:                            RO  stop ZigBee stack
config channel_change [channel]:        RO  configure channel change to channel number [11-26]
config concentrator [time] [radius]:    RO  configure device as concentrator, time: the time in seconds between concentrator route discoveries [0x0~0xFFFFFFFF], radius the hop count radius for concentrator route discoveries [0-255].
config ed_timeout [timeout]:            RO  configure network end device timeout [0-14] according to Requested Timeout enumerated values [10 sec ~ 16384 min]
config pan_channel [get|set] <ch>:      RO  get or set the pan channel (shared with OpenThread)
config power_capa:                      RO  get tx power capabilities
config rx_sensitivity:                  RO  get rx sensitivity
config tx_power <power_value>:          RO  configure or read tx power: [-20, 22] dBm
config tx_ppch [channel] <power_value>: RO  configure or read tx power per channel: [-20, 22] dBm
config manuf_code [manuf_code]:         RO  configure the manufacturer code: [0xMMMM]
config custom_msp [profile] [index]:    RO  register a custom manufacturer specific profile [0xC000~0xFFFF] at index [0~19]
config frag_on_msc [cluster] [index]:   RO  register a manufacturer specific cluster allowing APS fragmentation [0xFC00~0xFFFF] at index [0~19]
config get_version [version_type]:      R-  get software version [host|stack|firmware]
config trace [level] [mask]:            RO  configure the zboss log, level [0-4], mask [0x0~0xFFFFFFFF]
config redirect_trace [enable]:         RO  configure the zboss log, redirect to app if enabled [0|1]
                                            Note: only if ZB_TRACE_TO_FILE is enabled (case of Linux solutions)
config dbgtty [level]:                  RO  configure debug TTY frames, level: [0-3]


Menu network:
-------------

- network help:                    display network commands
- network form [mode]:             form network mode: [ centralized distributed ]
- network open [time]:             open network time: [ 0-255 ] seconds
- network ch_update [disable]:     configure disable channel change from unsolicited network updates enabled by default, disable [0(default)|1]
- network close:                   close network
- network join [mode]:             join network mode: [ centralized distributed ]
- network leave [rejoin]:          leave network, rejoin: [0-1]
- network get_source_route [addr]: get source route to a specific device at addr [0xAAAA]


Menu endpoint:
--------------

- endpoint help:                                      display endpoint commands
- endpoint create [id] [profile] [device] [version]:  create an endpoint id: [ 0-255 ] profile [ 0xPPPP or initials ], device [ 0xDDDD or initials ], version [ 0-255 ]
- endpoint print:                                     print the endpoint table

Notes:
- Endpoints must be created BEFORE starting the stack (config start)

To get the profiles list, just run the help command:
zbcli> endpoint create help


Menu cluster:
-------------

- cluster help:                                                   display cluster commands
- cluster create [endpoint] [cluster] [role]:                     create an cluster on endpoint id [0-255], cluster [0xCCCC or initials] role [ server client ]
- cluster bind [endpoint] [ieee] [short] [ep] [cluster] [mode]:
        bind a cluster on endpoint id [0-255] to a device at addr [IEEE] [0xSSSS] endpoint [0-255] cluster [0xCCCC or initials] mode [ieee|group]
- cluster unbind [endpoint] [ieee] [short] [ep] [cluster] [mode]:
        unbind a cluster on endpoint id [0-255] to a device at addr [IEEE] [0xSSSS] endpoint [0-255] cluster [0xCCCC or initials] mode [ieee|group]
- cluster disc_attr [endpoint] [addr] [ep] [cluster] [start] [max] <aps_secured> <disable_aps_ack> <delay>:
        discover attribute on endpoint id [0-255] to a device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] dest_ep [0-255] cluster [0xCCCC or initials] starting attr_id [0xAAAA] max number [0-255]
        extra options:  <aps_secured>:     enable  tx option Security transmission
        extra options:  <disable_aps_ack>: disable tx option Acknowledged transmission
        extra options:  <delay>:           delay the APS request
- cluster read_attr [endpoint] [addr] [ep] [cluster] [attr]:
        read attribute on endpoint id [0-255] to a device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] dest_ep [0-255] cluster [0xCCCC or initials] attribute  [0xAAAA]
- cluster write_attr [endpoint] [addr] [ep] [cluster] [attr] [type] [val]:
        write attribute on endpoint id [0-255] to a device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] dest_ep [0-255] cluster [0xCCCC or initials] attribute [0xAAAA] type [0xTT] val in hex [VV:VV....VV:VV]
- cluster read_local [endpoint] [cluster] [role] [attr]:
        read local attribute on endpoint id [0-255] cluster [0xCCCC or initials] role [client|server] attribute [0xAAAA]
- cluster write_local [endpoint] [cluster] [role] [attr] [type] [val]:
        write local attribute on endpoint id [0-255] cluster [0xCCCC or initials] role [client|server] attribute [0xAAAA] type [0xTT] val in hex [VV:VV....VV:VV]
- cluster raw_cmd [endpoint] [addr] [ep] [cluster] [cmd] [payload]:
        send ZCL Cluster command on endpoint id [0-255] to a device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] dest_ep [0-255] cluster [0xCCCC or initials] command [0xCC] optional payload
- cluster profile_cmd [endpoint] [addr] [ep] [cluster] [cmd] [payload]:
        send ZCL Profile-wide command on endpoint id [0-255] to a device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] dest_ep [0-255] cluster [0xCCCC or initials] command [0xCC] optional payload [P1:P2....Pn]
- cluster aps_cmd [endpoint] [addr] [ep] [cluster] [type] [cmd] <payload>:
        send APS ZCL command on endpoint id [0-255] to a device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] dest_ep [0-255] cluster [0xCCCC or initials] cmd type [0x00|0x01] command [0xCC] optional payload <P1:P2....Pn>
- cluster onoff_cmd:                                              SUBMENU ON/OFF Commands
- cluster identify_cmd:                                           SUBMENU IDENTIFY Commands
- cluster groups_cmd:                                             SUBMENU GROUPS Commands
- cluster scenes_cmd:                                             SUBMENU SCENES Commands
- cluster thermostat_cmd:                                         SUBMENU THERMOSTAT Commands
- cluster custom_nxp_cmd:                                         SUBMENU CUSTOM NXP Commands
- cluster ota_server:                                             SUBMENU OTA Upgrade Server
- cluster ota_client:                                             SUBMENU OTA Upgrade Client
- cluster print [endpoint]:                                       print the cluster table on endpoint id [0-255]

Notes:
- Clusters supports attributes access only, no commands are implemented unless specific <cluster>_cmd
- Clusters servers must be created before clusters clients, if not, an error Unauthorized will be returned
- Clusters must be created BEFORE starting the stack (config start)


SubMenu cluster onoff_cmd:
--------------------------

- cluster onoff_cmd on [endpoint] [dest_addr] <dest_ep>:
        send ZCL OnOff Command ON to a specific device at dest_addr|group_id [0xAAAA] optional endpoint <0-255> in case command is sent to a group
- cluster onoff_cmd off [endpoint] [dest_addr] <dest_ep>:
        send ZCL OnOff Command OFF to a specific device at dest_addr|group_id [0xAAAA] optional endpoint <0-255> in case command is sent to a group
- cluster onoff_cmd toggle [endpoint] [dest_addr] <dest_ep>:
        send ZCL OnOff Command TOGGLE to a specific device at dest_addr|group_id [0xAAAA] optional endpoint <0-255> in case command is sent to a group


SubMenu cluster identify_cmd:
-----------------------------

- cluster identify_cmd identify [endpoint] [dest_addr] [dest_ep] [time]:
        send ZCL Command identify from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] and set identification time [0-65535]
- cluster identify_cmd identify_query [endpoint] [dest_addr] [dest_ep]:
        send ZCL Command identify_query from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255]


SubMenu cluster groups_cmd:
--------------------------

- cluster groups_cmd add [endpoint] [dest_addr] [dest_ep] [group_id]:
        send ZCL Groups Command ADD from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]
- cluster groups_cmd view [endpoint] [dest_addr] [dest_ep]  [group_id]:
        send ZCL Groups Command VIEW from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]
- cluster groups_cmd get_membership [endpoint] [dest_addr] [dest_ep] [group_ids_list]:
        send ZCL Groups Command GET from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] list of 0~1 group_ids [0xGGGG]
- cluster groups_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id]:
        send ZCL Groups Command REMOVE from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]
- cluster groups_cmd remove [endpoint] [dest_addr] [dest_ep] all:
        send ZCL Groups Command REMOVE_ALL from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255]
- cluster groups_cmd add_if_ident [endpoint] [dest_addr] [dest_ep] [group_id]:
        send ZCL Groups Command ADD_IF_IDENTIFYING from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]


SubMenu cluster scenes_cmd:
---------------------------

- cluster scenes_cmd add [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id] [tr_time] { [cluster] [field_len] [field_data] }...:
        send ZCL Scenes Command ADD from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS] transition  [0-65535]
        Extension field sets: one per cluster: for Cluster id [0xCCCC or initials] field len [1-255] field set [0xFF....FF]
- cluster scenes_cmd view [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:
        send ZCL Scenes Command VIEW from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]
- cluster scenes_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:
        send ZCL Scenes Command REMOVE from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]
- cluster scenes_cmd remove [endpoint] [dest_addr] [dest_ep] [group_id] all:
        send ZCL Scenes Command REMOVE_ALL from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]
- cluster scenes_cmd store [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:
        send ZCL Scenes Command STORE from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]
- cluster scenes_cmd recall [endpoint] [dest_addr] [dest_ep] [group_id] [scene_id]:
        send ZCL Scenes Command RECALL from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG] scene_id [0xSS]
- cluster scenes_cmd get_membership [endpoint] [dest_addr] [dest_ep] [group_id]:
        send ZCL Scenes Command GET_MEMBERSHIP from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] group_id [0xGGGG]

Restrictions: do not use group_id: 0x0000 and scene_id: 0x00

SubMenu cluster thermostat_cmd:
-------------------------------

- cluster thermostat_cmd setpoint_raise_lower [endpoint] [dest_addr] [dest_ep] [mode] [amount]:
	send ZCL Command SETPOINT_RAISE_LOWER from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255] mode [heat|cool|both] the amount the setpoint(s) are to be increased (or decreased) by, in steps of 0.1 degrees celsius [(-128)-127]


SubMenu cluster custom_nxp_cmd:
-------------------------------

- cluster custom_nxp_cmd ping [endpoint] [dest_addr] [dest_ep] <number> <size> <interval>:
        send ZCL Command PING from endpoint [0-255] to a specific device at dest_addr [0xAAAA] endpoint [0-255], optionaly repeat number <1-65535> payload size <0-255> delay between cmds in ms <0-65535>
        default repeat number: 1
        default payload size:  40, min 34, start fragmenting at 69
        default delay:         1000
- cluster custom_nxp_cmd abort:
        stop ongoing ping
- cluster custom_nxp_cmd status:
        status ongoing ping


SubMenu cluster ota_<client|server>:
------------------------------------

- cluster ota_server add_file [endpoint] [file]:          add on endpoint id [0-255], file [<path>/<MMMM:manuf>-<TTTT:type>-<VVVVVVVV-version>-<name>.zigbee]
- cluster ota_client get_file [addr] [ep] [manuf] [type]: get file to server_addr [0xSSSS], endpoint_id [[0-255]] manufacturer [0xMMMM], image_type [0xTTTT]
- cluster ota_client accept_file [manuf] [type]:          accept file manufacturer [0xMMMM], image_type [0xTTTT] from frame ZCL OTA Image Notify
- cluster ota_client auto [enable]:                       do upgrade on reception of OTA Image Notify if enabled [0|1(default)]
- cluster ota_client tempo [duration]:                    temporisation in seconds before starting upgrade, tempo [1-60(default)-255]
- cluster ota_client status:                              get status of ota_upgrade
Notes:
- Clusters ota_<client|server> requires to have the stack started


Menu zdo:
---------

- zdo get_short_addr [ieee]:                                 get from local ieee_addr_table the specific device at addr [AA:AA:AA:AA:AA:AA:AA:AA]
- zdo get_ieee_addr [addr]:                                  send ZDO IEEE Address Req to a specific device at addr [0xAAAA]
- zdo get_active [addr]:                                     send ZDO Active Endpoint Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA]
- zdo get_bind_table [addr] [index]:                         send ZDO Mgmt Bind Req to a specific device at addr [0xAAAA] from index [0~255]
- zdo get_node_desc [addr]:                                  send ZDO Node Descritor Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA]
- zdo get_power_desc [addr]:                                 send ZDO Power Descritor Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA]
- zdo get_simple_desc [addr] [endpoint]:                     send ZDO Simple Descriptor Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] endpoint [0-255]
- zdo get_match_desc [addr] [profile] [#in:#out] [clusters]: send ZDO Match Descriptor Request to a specific device at dest_addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] profile [0xPPPP|initials] numbers of input:output clusters [0~255:0~255], at least one, list of clusters [0xCCC1|initials] ... [0xCCCn|initials] (n:#in + #out)
- zdo get_route [addr] [idx]:                                send ZDO Route Table Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] starting at index [0-255]
- zdo get_lqi [addr] [idx]:                                  send ZDO Link Quality Req to a specific device at addr [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] starting at index [0-255]
- zdo active_scan [duration] [channel_mask] [panid2join]:       send ZDO Active Scan Req for duration [0-14] on channel mask [0xMMMMMMMM], optionally panid to join [0xFFFF]
- zdo energy_scan [duration] [channel_mask]:                 send ZDO Energy Scan Req for duration [0-14] on channel mask [0xMMMMMMMM]
- zdo raw_cmd [addr] [cmd] <payload>:                        send ZDO command to a specific device at dest_addr short or ieee [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] command [0xCCCC] optional payload <P1:P2....Pn>
- zdo data_ind [enable]:                                     register ZDO Data Indication callback if enabled [0|1]
- zdo tsn_policy [tsn_policy]:                               configure ZDO tsn_policy [0: full range (default)|1: range 128_254]
- zdo aps_cmd [addr] [cmd] <payload>:                        send ZDO APS command to a specific device at dest_addr short or ieee [0xAAAA|AA:AA:AA:AA:AA:AA:AA:AA] command [0xCCCC] optional payload <P1:P2....Pn>


Menu installcode:
-----------------

- installcode policy [check]:            set installcode policy check: [enable|disable], ZC only
- installcode add [type] [code] [ieee]:  add installcode type: [48|64|96|128|passcode|hashed] code [IC:...:IC] for device addr [AA:AA:AA:AA:AA:AA:AA:AA], ZC only
- installcode remove [ieee]:             remove installcode for device addr [ieee|all], ZC only
- installcode list:                      list all installcode, ZC only
- installcode set [type] [code]:         set installcode type: [48|64|96|128|passcode|hashed] code [IC:...:IC], ZR ZED only



Menu nvram:
-----------

- nvram erase_at_start [erase]:                   erase nvram at start if erase [0|1] is 1, replace config factory_reset
- nvram app_register [dataset] [size]:            register nvram application dataset [1-4] for read/write, buffer size [size]
- nvram app_write [dataset] [offset] [payload]:   write to nvram application dataset [1-4] from data index [offset] payload [DD:...:DD]
- nvram app_read_local [dataset] [offset] [len]:  read local variable from nvram application dataset [1-4] from data index [offset] size [len]
- nvram buf_alloc [pool] [size] [method]:         test allocation io buffer from pool [out|any], size [0-max], method [get|init]
- nvram print :                                   print nvram & buffer information




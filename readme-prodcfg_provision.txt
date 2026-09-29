prodcfg_provision - IFR0 production-config provisioning utility (MCXW72)
=======================================================================

Purpose
-------
Standalone one-shot firmware app (no ZBOSS) that writes the Zigbee
production config into IFR0 as PLAINTEXT, at runtime, via the Zephyr flash API.
It is the runtime equivalent of the blhost write-memory provisioning steps in
nxp_delivery/README-zephyr.txt: the plaintext bytes are written verbatim.

Flow (main.c):
  1. Assemble the production-config body from the config fields and compute its
     CRC-32 automatically - the same way the ZBOSS stack validates it (reflected
     CRC-32, poly 0xEDB88320, init 0xFFFFFFFF, XorOut 0xFFFFFFFF; i.e. standard
     zlib/ISO 3309 CRC-32). The CRC covers the 345-byte body:
        len(2) + version(2) + channel_masks(40) + ieee_addr(8) + tx_power(270) +
        options(1) + install_code(18) + passcode(4)
  2. Build the 8 KB IFR0 sector image and lay in the three plaintext sections:
        ieee802154_eui64 @ 0x000 :  8 bytes  IEEE MAC address
        zb_settings      @ 0x010 : header (computed CRC / len / ver) + channel
                                    masks + tx power + options
        zb_secured       @ 0x200 : install code(18) + passcode(4)
  3. flash_erase the 8 KB sector, then flash_write the whole image back.

Configuration
-------------
All values the user needs to set are grouped in a clearly marked
"USER CONFIGURATION - EDIT HERE" section at the top of src/main.c
(ieee_addr, CHANNEL, TXPOWER_DBM, options, install_code, passcode).
Everything below that section is generic and does not need to be touched.
The header CRC-32 is computed at runtime from these values - no manual CRC entry.

Options byte
------------
The options byte (zb_production_config_ver_4_t.options) is a bitfield:
    bits [1:0] IC_TYPE  - install-code type:
                            0 = 48-bit,  1 = 64-bit,
                            2 = 96-bit,  3 = 128-bit (16-byte code)
    bit  6     PASSCODE_PRESENT (0x40) - set if a passcode is provided
    bit  7     CERT_PRESENT     (0x80) - certificate block present
                                         (not written by this app -> leave 0)

Typical values to send:
    0x00  no install code, no passcode (example config)
    0x03  128-bit (16-byte) install code, no passcode
    0x43  128-bit install code + passcode present (0x03 | 0x40)
    0x40  passcode only, no install code

An all-zero install_code is treated by the stack as "not set" regardless of
IC_TYPE, so the example config uses 0x00.

The user does not need to enter the header CRC-32 manually - it is computed
automatically by the app from the config values.

WARNING: run this once, under controlled power. The whole 8 KB IFR0 sector is
erased and rewritten; a power loss between erase and write-back corrupts IFR0.

After running this app, reset the board and load the Zigbee firmware. On the next
restart you should see "signal 23: ZDO PRODUCTION_CONFIG_READY, status 0".

Notes
-----
- Plaintext only: no key wrapping, no secure storage.

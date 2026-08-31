/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * prodcfg_provision - one-shot IFR0 production-config provisioning utility.
 *
 * Writes the Zigbee production config into IFR0 as PLAINTEXT, at runtime, via
 * the Zephyr flash API. The plaintext bytes are written verbatim. The
 * zb_settings header CRC-32 is computed automatically the same way
 * the ZBOSS stack validates it (zb_production_config_calculate_crc /
 * zb_crc32_next: reflected CRC-32, poly 0xEDB88320, init 0xFFFFFFFF, XorOut
 * 0xFFFFFFFF - i.e. standard zlib/ISO 3309 CRC-32).
 *
 * IFR0 layout (MCXW72), 8 KB sector at 0x2002000:
 *   ieee802154_eui64 @ 0x2002000 (offset 0x000):  8 bytes  IEEE MAC address
 *   zb_settings      @ 0x2002010 (offset 0x010): header + channel masks +
 *                                                 tx power + options
 *   zb_secured       @ 0x2002200 (offset 0x200): install code(18) + passcode(4)
 *
 * The CRC-32 is computed over the assembled production-config body (345 bytes):
 *   len(2) + version(2) + channel_masks(40) + ieee_addr(8) + tx_power(270) +
 *   options(1) + install_code(18) + passcode(4)
 * and stored little-endian at offset 0 of zb_settings.
 *
 * WARNING: run once, under controlled power. A power loss between erase and
 * write-back leaves IFR0 blank/corrupt.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <string.h>
#include <stdbool.h>

#define IFR0_FLASH_NODE   DT_PARENT(DT_NODELABEL(ifr0))
#define IFR0_SECTOR_SIZE  0x2000u        /* 8 KB erase/write sector           */
#define IFR0_BASE_OFF     0x2002000u     /* absolute base of the IFR0 sector  */

#define EUI64_OFF         0x000u
#define ZB_SETTINGS_OFF   0x010u
#define ZB_SECURED_OFF    0x200u

/* Production config sizes (version 4). */
#define PC_VERSION        0x0004u
#define N_CHANNEL_MASKS   10u             /* 10 x 4-byte page/mask entries     */
#define MASKS_LEN         (N_CHANNEL_MASKS * 4u)   /* 40                       */
#define TXPOWER_LEN       270u
#define IC_LEN            18u             /* install code (16B) + CRC-16 (2B)  */
#define PC_LEN            4u              /* passcode                          */

static const struct device *const ifr0_flash = DEVICE_DT_GET(IFR0_FLASH_NODE);

#define ALIGNED __attribute__((aligned(4)))

/*
 * ============================================================================
 *                       USER CONFIGURATION - EDIT HERE
 * ============================================================================
 * Set the production-config values for your device below. Everything else in
 * this file is generic and does not need to be touched. The user does not need
 * to enter the header CRC-32 manually - it is computed automatically from these
 * values at runtime.
 *
 * The defaults match the README-zephyr.txt example config.
 */

/* 1. IEEE 802.15.4 extended (MAC) address, 8 bytes, stored LSB-first in IFR0.
 *    The stack/CLI display the EUI in the REVERSE order: the bytes below,
 *    { 0x11..0x88 }, are read back as EUI 88:77:66:55:44:33:22:11. When adding
 *    the matching install code on the coordinator ("installcode add ... <ieee>")
 *    use that displayed order (88:77:...:11), or the TC key lookup will fail. */
static const uint8_t ieee_addr[8] = {
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
};

/* 2. 2.4 GHz (page 0) channel: 11..26. The channel mask is bit N = channel N
 *    (mask = 1 << CHANNEL), so channel 11 -> 0x00000800, channel 21 -> 0x00200000.
 *    It is placed little-endian in page-0 (entry 0) of the 10-entry mask list;
 *    the other 9 (sub-GHz) entries stay 0. */
#define CHANNEL           11

/* 3. TX power in dBm, applied to all 270 channel entries. */
#define TXPOWER_DBM       7

/*
 * 4. Options byte. Bit layout used by the stack
 *    (zb_production_config_ver_4_t.options, see zboss_api.h):
 *      bits [1:0] IC_TYPE  - install-code type:
 *                              0 = 48-bit,  1 = 64-bit,
 *                              2 = 96-bit,  3 = 128-bit (16-byte code)
 *      bit  6     PASSCODE_PRESENT (0x40) - set if a passcode is provided
 *      bit  7     CERT_PRESENT     (0x80) - set if a certificate block follows
 *                                           (not written by this app -> leave 0)
 *    What to send:
 *      - No install code / no passcode (example) ............ 0x00
 *      - 128-bit (16-byte) install code, no passcode ........ 0x03
 *      - 128-bit install code + passcode present ............ 0x43 (0x03 | 0x40)
 *      - passcode only, no install code ..................... 0x40
 *    An all-zero install_code below is treated by the stack as "not set"
 *    regardless of IC_TYPE, so the example uses 0x00.
 */
static const uint8_t options = 0x43;

/* 5. Install code: supply ONLY the raw code bytes here (their length is set by
 *    IC_TYPE in options: 48->6, 64->8, 96->12, 128->16 bytes). The app appends
 *    the 2-byte CRC-16 automatically, so you do not set the CRC manually.
 *    Leave all-zero for "no install code" (see options above). */
static const uint8_t install_code[IC_LEN] = {
	0x01, 0x02, 0x20, 0x33, 0x30, 0x44, 0x01, 0x02,
	0x20, 0x33, 0x30, 0x44, 0x01, 0x02, 0x20, 0x33,
};

/* 6. Passcode (R23), 4 bytes. Only meaningful if PASSCODE_PRESENT (0x40) is set
 *    in options. All-zero = no passcode. */
static const uint8_t passcode[PC_LEN] = { 0x01, 0x02, 0x03, 0x04 };

/*
 * ============================================================================
 *                    END OF USER CONFIGURATION - do not edit below
 * ============================================================================
 */

/* ZBOSS production-config CRC-32 (reflected, poly 0xEDB88320, init 0xFFFFFFFF,
 * XorOut 0xFFFFFFFF), identical to the stack's zb_crc32(). */
static uint32_t pc_crc32(const uint8_t *p, size_t len)
{
	uint32_t crc = 0xFFFFFFFFu;

	for (size_t i = 0; i < len; i++) {
		crc ^= p[i];
		for (int j = 0; j < 8; j++) {
			uint32_t mask = (crc & 1u) ? 0xFFFFFFFFu : 0u;
			crc = (crc >> 1) ^ (0xEDB88320u & mask);
		}
	}
	return ~crc;
}

/* Install-code CRC-16 (reflected, poly 0x8408, init 0xFFFF, XorOut 0xFFFF),
 * identical to the stack's ~zb_crc16(ic, 0xffff, len). */
static uint16_t ic_crc16(const uint8_t *p, size_t len)
{
	uint16_t crc = 0xFFFFu;

	for (size_t i = 0; i < len; i++) {
		crc ^= p[i];
		for (int j = 0; j < 8; j++) {
			crc = (crc & 1u) ? (uint16_t)((crc >> 1) ^ 0x8408u)
					 : (uint16_t)(crc >> 1);
		}
	}
	return (uint16_t)~crc;
}

/* Install-code length (bytes) from IC_TYPE (options[1:0]): 48/64/96/128-bit. */
static size_t ic_code_len(uint8_t opts)
{
	static const uint8_t by_type[4] = { 6u, 8u, 12u, 16u };

	return by_type[opts & 0x03u];
}

static void hexdump(const char *tag, const uint8_t *b, size_t n)
{
	printk("%s (%u):", tag, (unsigned)n);
	for (size_t i = 0; i < n; i++) {
		printk(" %02x", b[i]);
	}
	printk("\n");
}

int main(void)
{
	static ALIGNED uint8_t sector[IFR0_SECTOR_SIZE];
	/* Assembled config body over which the CRC-32 is computed (345 bytes):
	 * len(2) + version(2) + masks(40) + ieee(8) + txpower(270) + options(1) +
	 * install_code(18) + passcode(4). */
	uint8_t body[2 + 2 + MASKS_LEN + 8 + TXPOWER_LEN + 1 + IC_LEN + PC_LEN];
	uint8_t channel_masks[MASKS_LEN] = { 0 };
	uint8_t ic[IC_LEN] = { 0 };                      /* install code + CRC-16     */
	uint32_t page0_mask = (uint32_t)1u << CHANNEL;   /* page 0, bit N = channel N */
	uint8_t *p = body;
	uint16_t len;
	uint32_t crc;
	int ret;

	printk("\n=== prodcfg_provision: plaintext production config -> IFR0 ===\n");

	if (!device_is_ready(ifr0_flash)) {
		printk("ERROR: IFR0 flash device not ready\n");
		return -1;
	}

	/* Page-0 channel mask, little-endian, in entry 0 of the mask list. */
	channel_masks[0] = (uint8_t)(page0_mask & 0xFFu);
	channel_masks[1] = (uint8_t)((page0_mask >> 8) & 0xFFu);
	channel_masks[2] = (uint8_t)((page0_mask >> 16) & 0xFFu);
	channel_masks[3] = (uint8_t)((page0_mask >> 24) & 0xFFu);

	/* Install code: copy the user code bytes and append the CRC-16 (little-endian)
	 * over the IC_TYPE-sized code. An all-zero code stays all-zero (= "no install
	 * code"), so no CRC is appended in that case. */
	memcpy(ic, install_code, IC_LEN);
	{
		size_t clen = ic_code_len(options);
		bool have_ic = false;

		for (size_t i = 0; i < clen; i++) {
			if (ic[i] != 0u) { have_ic = true; break; }
		}
		if (have_ic) {
			uint16_t c16 = ic_crc16(ic, clen);

			ic[clen]     = (uint8_t)(c16 & 0xFFu);
			ic[clen + 1] = (uint8_t)((c16 >> 8) & 0xFFu);
		}
	}

	/* Build the CRC body. len is the whole config length including the 4-byte
	 * CRC field (i.e. sizeof(body) + 4). */
	len = (uint16_t)(sizeof(body) + 4u);
	*p++ = (uint8_t)(len & 0xFFu);
	*p++ = (uint8_t)(len >> 8);
	*p++ = (uint8_t)(PC_VERSION & 0xFFu);
	*p++ = (uint8_t)(PC_VERSION >> 8);
	memcpy(p, channel_masks, MASKS_LEN);      p += MASKS_LEN;
	memcpy(p, ieee_addr, sizeof(ieee_addr));  p += sizeof(ieee_addr);
	memset(p, (uint8_t)TXPOWER_DBM, TXPOWER_LEN); p += TXPOWER_LEN;
	*p++ = options;
	memcpy(p, ic, IC_LEN);                    p += IC_LEN;
	memcpy(p, passcode, PC_LEN);              p += PC_LEN;

	crc = pc_crc32(body, sizeof(body));
	printk("computed prod-config CRC-32 = 0x%08x, len = %u\n", crc, len);

	/* Build the 8 KB IFR0 sector image (erased 0xFF background), then lay in the
	 * three plaintext sections. */
	memset(sector, 0xFF, sizeof(sector));

	/* ieee802154_eui64 @ 0x000 */
	memcpy(&sector[EUI64_OFF], ieee_addr, sizeof(ieee_addr));

	/* zb_settings @ 0x010: header (CRC/len/ver) + channel masks + tx power +
	 * options. The header's 8 bytes then the body up to (but not including) the
	 * ieee/install/passcode fields, which live in the eui64/zb_secured sections. */
	{
		uint8_t *s = &sector[ZB_SETTINGS_OFF];

		s[0] = (uint8_t)(crc & 0xFFu);
		s[1] = (uint8_t)((crc >> 8) & 0xFFu);
		s[2] = (uint8_t)((crc >> 16) & 0xFFu);
		s[3] = (uint8_t)((crc >> 24) & 0xFFu);
		s[4] = (uint8_t)(len & 0xFFu);
		s[5] = (uint8_t)(len >> 8);
		s[6] = (uint8_t)(PC_VERSION & 0xFFu);
		s[7] = (uint8_t)(PC_VERSION >> 8);
		memcpy(&s[8], channel_masks, MASKS_LEN);
		memset(&s[8 + MASKS_LEN], (uint8_t)TXPOWER_DBM, TXPOWER_LEN);
		s[8 + MASKS_LEN + TXPOWER_LEN] = options;
	}

	/* zb_secured @ 0x200: install code (18B) + passcode (4B) */
	memcpy(&sector[ZB_SECURED_OFF], ic, IC_LEN);
	memcpy(&sector[ZB_SECURED_OFF + IC_LEN], passcode, PC_LEN);

	hexdump("eui64",        &sector[EUI64_OFF],       sizeof(ieee_addr));
	hexdump("zb_settings hdr", &sector[ZB_SETTINGS_OFF], 8u);
	hexdump("zb_secured",   &sector[ZB_SECURED_OFF],  IC_LEN + PC_LEN);

	/* Erase then write the whole IFR0 sector in one shot. */
	ret = flash_erase(ifr0_flash, IFR0_BASE_OFF, IFR0_SECTOR_SIZE);
	if (ret != 0) {
		printk("ERROR: flash_erase(IFR0) = %d\n", ret);
		return ret;
	}
	ret = flash_write(ifr0_flash, IFR0_BASE_OFF, sector, sizeof(sector));
	if (ret != 0) {
		printk("ERROR: flash_write(IFR0) = %d\n", ret);
		return ret;
	}

	printk("OK: IFR0 provisioned with plaintext production config "
	       "(CRC 0x%08x). Reset and run the Zigbee firmware.\n", crc);
	return 0;
}

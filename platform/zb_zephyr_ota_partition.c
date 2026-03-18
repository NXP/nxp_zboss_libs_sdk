/*
 * Copyright 2026 NXP
 *
 * NXP Proprietary.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 *
 */

#include <zephyr/storage/flash_map.h>
#include <string.h>

uint8_t zephyr_ota_get_partition_id(const char *partition_name)
{
	if (strcmp(partition_name, "slot1_partition") == 0) {
		return FIXED_PARTITION_ID(slot1_partition);
	} else if (strcmp(partition_name, "slot0_partition") == 0) {
		return FIXED_PARTITION_ID(slot0_partition);
	} else if (strcmp(partition_name, "storage_partition") == 0) {
		return FIXED_PARTITION_ID(storage_partition);
	}
	return 0; /* Invalid partition */
}
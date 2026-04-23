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
    return  PARTITION_ID(slot1_partition);
  } else if (strcmp(partition_name, "slot0_partition") == 0) {
    return  PARTITION_ID(slot0_partition);
  } else if (strcmp(partition_name, "storage_partition") == 0) {
    return  PARTITION_ID(storage_partition);
  }
  return 0; /* Invalid partition */
}


const struct device *zephyr_ota_get_device_pointer(const char *partition_name, uint32_t *offset, uint32_t *size)
{
  const struct flash_area *fa;
  const struct device *dev = NULL;

  uint8_t partition_id = zephyr_ota_get_partition_id(partition_name);
  if (partition_id == 0) {
    return NULL;
  }

  int err = flash_area_open(partition_id, &fa);
  if (err) {
    return NULL;
  }

  dev = flash_area_get_device(fa);

  if (offset != NULL) {
    *offset = fa->fa_off;
  }

  if (size != NULL) {
    *size = fa->fa_size;
  }

  flash_area_close(fa);

  return dev;
}

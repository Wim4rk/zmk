/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/init.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/*
 * The STM32WB IPM driver writes the UID-derived public address before the
 * host's HCI reset, which reverts it, so every board ends up with the stack
 * default 02:80:E1:00:00:00. Instead, create a static random identity derived
 * from the chip's 96-bit unique ID: unique per chip, and stable even if the
 * settings partition is wiped. Must run before zmk_ble_init() calls bt_enable().
 */
#define AARDVARK_68_BLE_ADDR_INIT_PRIORITY 49
BUILD_ASSERT(AARDVARK_68_BLE_ADDR_INIT_PRIORITY < CONFIG_ZMK_BLE_INIT_PRIORITY,
             "identity must be created before bt_enable()");

static int aardvark_68_ble_addr_init(void) {
    uint8_t uid[12];
    ssize_t len = hwinfo_get_device_id(uid, sizeof(uid));

    if (len <= 0) {
        LOG_ERR("Failed to read chip UID (%d), using controller default address", (int)len);
        return 0;
    }

    /* FNV-1a 64 */
    uint64_t hash = 0xcbf29ce484222325ULL;
    for (ssize_t i = 0; i < len; i++) {
        hash ^= uid[i];
        hash *= 0x100000001b3ULL;
    }

    bt_addr_le_t addr = {.type = BT_ADDR_LE_RANDOM};
    for (int i = 0; i < 6; i++) {
        addr.a.val[i] = (uint8_t)(hash >> (8 * i));
    }
    BT_ADDR_SET_STATIC(&addr.a);

    int err = bt_id_create(&addr, NULL);
    if (err < 0) {
        LOG_ERR("Failed to create BLE identity (%d)", err);
    }

    return 0;
}

SYS_INIT(aardvark_68_ble_addr_init, APPLICATION, AARDVARK_68_BLE_ADDR_INIT_PRIORITY);

# SPDX-License-Identifier: MIT

target_sources(app PRIVATE aardvark_68_hse_tune.c)

if(CONFIG_ZMK_BLE)
  target_sources(app PRIVATE aardvark_68_ble_addr.c)
endif()

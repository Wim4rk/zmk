/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/init.h>
#include <stm32wbxx_ll_rcc.h>

/*
 * The 32 MHz HSE crystal is the BLE radio's frequency reference, and BLE
 * allows at most +-50 ppm. ST boards get their HSETUNE (internal load
 * capacitance) from OTP; this board has none, so it runs at the reset value
 * 0, which measured +44 ppm on rev 1.0.0 and made connections drop with a
 * supervision timeout right after connecting. 44 measured +4 ppm (TIM2 on
 * HSE vs. an NTP-disciplined host clock over SWD). Re-measure if the crystal
 * or its load capacitors change.
 */
#define AARDVARK_68_HSE_TUNE 44

static int aardvark_68_hse_tune_init(void) {
    LL_RCC_HSE_SetCapacitorTuning(AARDVARK_68_HSE_TUNE);
    return 0;
}

SYS_INIT(aardvark_68_hse_tune_init, PRE_KERNEL_1, 0);

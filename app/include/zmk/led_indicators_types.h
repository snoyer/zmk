/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define ZMK_LED_INDICATORS_NUMLOCK_BIT 0x01
#define ZMK_LED_INDICATORS_CAPSLOCK_BIT 0x02
#define ZMK_LED_INDICATORS_SCROLLLOCK_BIT 0x04
#define ZMK_LED_INDICATORS_COMPOSE_BIT 0x08
#define ZMK_LED_INDICATORS_KANA_BIT 0x10

typedef uint8_t zmk_led_indicators_flags_t;

/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/profiles_types.h>

#define ZMK_USB_PROFILE_COUNT (COND_CODE_1(IS_ENABLED(CONFIG_ZMK_USB), (1), (0)))
#define ZMK_PROFILE_COUNT (ZMK_USB_PROFILE_COUNT + ZMK_BLE_PROFILE_COUNT)

zmk_profile_index_t zmk_profile_index(enum zmk_endpoint endpoint, uint8_t profile);
zmk_profile_index_t zmk_current_profile_index();

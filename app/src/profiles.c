/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/profiles.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

zmk_profile_index_t zmk_profile_index(enum zmk_endpoint endpoint, uint8_t profile) {
    switch (endpoint) {
    case ZMK_ENDPOINT_USB:
        return 0;
    case ZMK_ENDPOINT_BLE:
        return ZMK_USB_PROFILE_COUNT + profile;
    }

    CODE_UNREACHABLE;
}

zmk_profile_index_t zmk_current_profile_index() {
    const enum zmk_endpoint endpoint = zmk_endpoints_selected();
    uint8_t profile = 0;

#if IS_ENABLED(CONFIG_ZMK_BLE)
    if (endpoint == ZMK_ENDPOINT_BLE) {
        profile = zmk_ble_active_profile_index();
    }
#endif

    const zmk_profile_index_t index = zmk_profile_index(endpoint, profile);
    LOG_DBG("endpoint=%d, profile=%d => profile_index=%d", endpoint, profile, index);
    return index;
}

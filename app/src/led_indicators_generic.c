/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <kernel.h>
#include <logging/log.h>

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/led_indicators.h>
#include <zmk/profiles.h>
#include <zmk/events/led_indicators_changed.h>
#include <zmk/events/endpoint_selection_changed.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static zmk_led_indicators_flags_t led_indicators_flags[ZMK_PROFILE_COUNT] = {0};
static bool led_indicators_flags_is_from_host[ZMK_PROFILE_COUNT] = {false};

zmk_led_indicators_flags_t zmk_led_indicators_get_current_flags() {
    const size_t index = zmk_current_profile_index();
    return led_indicators_flags[index];
}

zmk_led_indicators_flags_t zmk_led_indicators_get_flags(zmk_profile_index_t profile_index) {
    return led_indicators_flags[profile_index];
}

static void raise_led_indicators_changed_event(struct k_work *_work) {
    const size_t profile_index = zmk_current_profile_index();
    ZMK_EVENT_RAISE(new_zmk_led_indicators_changed((struct zmk_led_indicators_changed){
        .leds = led_indicators_flags[profile_index],
        .is_from_host = led_indicators_flags_is_from_host[profile_index],
    }));
}

static K_WORK_DEFINE(led_indicators_changed_work, raise_led_indicators_changed_event);

void zmk_led_indicators_update_flags_from_host(zmk_led_indicators_flags_t leds,
                                               enum zmk_endpoint endpoint, uint8_t profile) {
    const zmk_profile_index_t profile_index = zmk_profile_index(endpoint, profile);

    led_indicators_flags[profile_index] = leds;
    led_indicators_flags_is_from_host[profile_index] = true;

    k_work_submit(&led_indicators_changed_work);

    LOG_DBG("leds for profile %d reported to be 0x%x", profile_index, leds);
}

void zmk_led_indicators_update_flags_from_self(zmk_led_indicators_flags_t leds,
                                               zmk_profile_index_t profile_index) {
    led_indicators_flags[profile_index] = leds;
    led_indicators_flags_is_from_host[profile_index] = false;

    k_work_submit(&led_indicators_changed_work);

    LOG_DBG("leds for profile %d expected to be 0x%x", profile_index, leds);
}

void zmk_led_indicators_process_report(struct zmk_hid_led_report_body *report,
                                       enum zmk_endpoint endpoint, uint8_t profile) {
    const zmk_led_indicators_flags_t leds = report->leds;
    zmk_led_indicators_update_flags_from_host(leds, endpoint, profile);
}

static int led_indicators_endpoint_listener(const zmk_event_t *eh) {
    raise_led_indicators_changed_event(NULL);
    return ZMK_EV_EVENT_BUBBLE;
}
static ZMK_LISTENER(led_indicators_endpoint_listener, led_indicators_endpoint_listener);
static ZMK_SUBSCRIPTION(led_indicators_endpoint_listener, zmk_endpoint_selection_changed);

static int led_indicators_keycode_state_changed_listener(const zmk_event_t *eh) {
    struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (ev == NULL || !ev->state) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->usage_page == HID_USAGE_KEY) {
        zmk_led_indicators_flags_t flip_mask = 0;

        switch (ev->keycode) {
        case HID_USAGE_KEY_KEYPAD_NUM_LOCK_AND_CLEAR:
            flip_mask = ZMK_LED_INDICATORS_NUMLOCK_BIT;
            break;
        case HID_USAGE_KEY_KEYBOARD_CAPS_LOCK:
            flip_mask = ZMK_LED_INDICATORS_CAPSLOCK_BIT;
            break;
        case HID_USAGE_KEY_KEYBOARD_SCROLL_LOCK:
            flip_mask = ZMK_LED_INDICATORS_SCROLLLOCK_BIT;
            break;
        default:
            break;
        }

        if (flip_mask) {
            const zmk_profile_index_t profile_index = zmk_current_profile_index();
            zmk_led_indicators_update_flags_from_self(
                led_indicators_flags[profile_index] ^ flip_mask, profile_index);
        }
    }

    return ZMK_EV_EVENT_BUBBLE;
}
static ZMK_LISTENER(led_indicators_keycode_state_changed_listener,
                    led_indicators_keycode_state_changed_listener);
static ZMK_SUBSCRIPTION(led_indicators_keycode_state_changed_listener, zmk_keycode_state_changed);

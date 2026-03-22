/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
    zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
    lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_LEFT, 0,
                 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                               lv_theme_get_font_small(screen), LV_PART_MAIN);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_TESTING_SHAPSHOTS)
    zmk_display_status_screen_test();
#endif

    return screen;
}

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_TESTING_SHAPSHOTS)

void display_status_screen_test() {
    void wait_for_refresh() { k_sleep(K_MSEC(100)); }

    battery_status_update_cb((struct battery_status_state){.level = 0, .usb_present = true});
    layer_status_update_cb((struct layer_status_state){.label = "qwerty"});
    output_status_update_cb((struct output_status_state){});
    wait_for_refresh();

    uint8_t levels[] = {100, 70, 40, 10, 0};
    for (int i = 0; i < sizeof(levels); i++) {
        LOG_DBG("battery = %d%%", levels[i]);
        battery_status_update_cb((struct battery_status_state){.level = levels[i]});
        wait_for_refresh();
    }

    LOG_DBG("BLE preferred, USB selected");
    output_status_update_cb((struct output_status_state){
        .selected_endpoint = {ZMK_TRANSPORT_USB},
        .preferred_transport = ZMK_TRANSPORT_BLE,
        .active_profile_connected = false,
        .active_profile_bonded = true,
    });
    wait_for_refresh();

    LOG_DBG("BLE preferred, BLE 1 selected, not bonded");
    output_status_update_cb((struct output_status_state){
        .selected_endpoint = {ZMK_TRANSPORT_BLE, .ble = {0}},
        .preferred_transport = ZMK_TRANSPORT_BLE,
        .active_profile_connected = false,
        .active_profile_bonded = false,
    });
    wait_for_refresh();

    LOG_DBG("BLE preferred, BLE 2 selected, not connected");
    output_status_update_cb((struct output_status_state){
        .selected_endpoint = {ZMK_TRANSPORT_BLE, .ble = {1}},
        .preferred_transport = ZMK_TRANSPORT_BLE,
        .active_profile_connected = false,
        .active_profile_bonded = true,
    });
    wait_for_refresh();

    LOG_DBG("BLE preferred, BLE 3 selected, connected");
    output_status_update_cb((struct output_status_state){
        .selected_endpoint = {ZMK_TRANSPORT_BLE, .ble = {2}},
        .preferred_transport = ZMK_TRANSPORT_BLE,
        .active_profile_connected = true,
        .active_profile_bonded = true,
    });
    wait_for_refresh();
}
void display_status_screen_test_cb(struct k_work *work) { display_status_screen_test(); }
static K_WORK_DELAYABLE_DEFINE(display_status_screen_test_work, display_status_screen_test_cb);
void zmk_display_status_screen_test() {
    k_work_schedule(&display_status_screen_test_work, K_MSEC(100));
}

#endif // IS_ENABLED(CONFIG_ZMK_DISPLAY_TESTING_SHAPSHOTS)
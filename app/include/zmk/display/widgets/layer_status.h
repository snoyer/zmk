/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/keymap.h>

struct zmk_widget_layer_status {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget);

struct layer_status_state {
    zmk_keymap_layer_index_t index;
    const char *label;
};
void layer_status_update_cb(struct layer_status_state state);

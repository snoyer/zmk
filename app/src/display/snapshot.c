
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <stdio.h>

#include <zephyr/devicetree.h>
#define DISPLAY_X_RES DT_PROP(DT_CHOSEN(zephyr_display), width)
#define DISPLAY_Y_RES DT_PROP(DT_CHOSEN(zephyr_display), height)

#include <lvgl.h>

uint32_t FNV1_hash(uint8_t *data, size_t len) {
    uint32_t hash = 2166136261;
    for (int i = 0; i < len; i++)
        hash = (hash ^ data[i]) * 16777619;
    return hash;
}

void print_snapshot(lv_obj_t *screen) {
    const char *SEXTANTS[64] = {" ", "🬞", "🬏", "🬭", "🬇", "🬦", "🬖", "🬵", "🬃", "🬢", "🬓", "🬱", "🬋",
                                "🬩", "🬚", "🬹", "🬁", "🬠", "🬑", "🬯", "🬉", "▐", "🬘", "🬷", "🬅", "🬤",
                                "🬔", "🬳", "🬍", "🬫", "🬜", "🬻", "🬀", "🬟", "🬐", "🬮", "🬈", "🬧", "🬗",
                                "🬶", "🬄", "🬣", "▌", "🬲", "🬌", "🬪", "🬛", "🬺", "🬂", "🬡", "🬒", "🬰",
                                "🬊", "🬨", "🬙", "🬸", "🬆", "🬥", "🬕", "🬴", "🬎", "🬬", "🬝", "█"};

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, DISPLAY_X_RES, DISPLAY_Y_RES, LV_COLOR_FORMAT_RGB888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

    static uint32_t prev_hash = 0;

    const int w = lv_obj_get_width(screen);
    const int h = lv_obj_get_height(screen);

    bool get_px(int x, int y) {
        return (x >= 0 && y >= 0 && x < w && y < h) &&
               (*(uint8_t *)lv_draw_buf_goto_xy(&draw_buf, x, y)) < 128;
    }

    if (lv_snapshot_take_to_draw_buf(screen, LV_COLOR_FORMAT_RGB888, &draw_buf) != LV_RES_INV) {

        const uint32_t hash = FNV1_hash(draw_buf.data, draw_buf.data_size);
        if (hash == prev_hash)
            return;
        prev_hash = hash;

        LOG_INF("%dx%d lvgl snapshot...", w, h);
        for (int y = 0; y < h; y += 3) {
            char line[(MAX(DISPLAY_X_RES, DISPLAY_Y_RES) / 2 + 1) * 4] = {0};
            char *line_ = line;
            for (int x = 0; x < w; x += 2) {
                const char *s = SEXTANTS[(get_px(x + 0, y + 0) << 5) + (get_px(x + 1, y + 0) << 4) +
                                         (get_px(x + 0, y + 1) << 3) + (get_px(x + 1, y + 1) << 2) +
                                         (get_px(x + 0, y + 2) << 1) + (get_px(x + 1, y + 2) << 0)];
                strncat(line_, s, 4);
            }
            printf("lvgl snapshot: %s\n", line); // LOG_xxx() mangles some characters
        }
    } else {
        LOG_WRN("snapshot failed");
    }
}

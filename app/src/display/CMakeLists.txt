# Copyright (c) 2020 The ZMK Contributors
# SPDX-License-Identifier: MIT

target_sources_ifdef(CONFIG_ZMK_DISPLAY app PRIVATE main.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_BUILT_IN app PRIVATE status_screen.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_TESTING_SHAPSHOTS app PRIVATE snapshot.c)

add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY widgets/)

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "libs/eadk.h"

char* input(void); // return the char mapped to the key

extern eadk_keyboard_state_t state;

extern bool is_shift;
extern bool is_alpha;

/*
    #define INPUT_BUFFER_CAPACITY 16

    char buffer[INPUT_BUFFER_CAPACITY] = {0}; // 15 chars max + '\0'
    while (!eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_ok)) {
        char* result = input();
        if (result) {
            if (result[0] == '\b') {
                size_t len = strlen(buffer);
                if (len > 0) buffer[len - 1] = '\0';
            } else {
                strncat(buffer, result, sizeof(buffer) - strlen(buffer) - 1);
            }
            eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
            eadk_display_draw_string(buffer, (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);

            eadk_timing_msleep(300);
        }
    }
*/
#endif
#include "eadk.h"

/*---------------------------------------------------------------------------*/

// All function for the input : 

// Mapping of keys to characters
typedef struct {
    eadk_key_t key;
    char normal;
    char alpha;
} KeyMapping;

// Mapping of keys to characters
static const KeyMapping key_mappings[] = {
    {eadk_key_one, '1', 'w'},
    {eadk_key_two, '2', 'x'},
    {eadk_key_three, '3', 'y'},
    {eadk_key_four, '4', 'r'},
    {eadk_key_five, '5', 's'},
    {eadk_key_six, '6', 't'},
    {eadk_key_seven, '7', 'm'},
    {eadk_key_eight, '8', 'n'},
    {eadk_key_nine, '9', 'o'},
    {eadk_key_zero, '0', '?'},
    {eadk_key_exp, '[', 'a'},
    {eadk_key_ln, ']', 'b'},
    {eadk_key_log, '{', 'c'},
    {eadk_key_imaginary, '}', 'd'},
    {eadk_key_comma, '_', 'e'},
    {eadk_key_power, ',', 'f'},
    {eadk_key_pi, '=', 'j'},
    {eadk_key_sqrt, '<', 'k'},
    {eadk_key_square, '>', 'l'},
    {eadk_key_left_parenthesis, '(', 'p'},
    {eadk_key_right_parenthesis, ')', 'q'},
    {eadk_key_multiplication, '*', 'u'},
    {eadk_key_division, '/', 'v'},
    {eadk_key_plus, '+', 'z'},
    {eadk_key_minus, '-', ' '},
    {eadk_key_dot, '.', '!'},
    {eadk_key_sine, 0, 'g'},
    {eadk_key_cosine, 0, 'h'},
    {eadk_key_tangent, 0, 'i'}
};

// refresh the screen with the buffer and the alpha mode for input()
void refresh_input(const char *buffer, size_t index, eadk_point_t point, bool is_alpha, bool draw_string, bool large_font) {
    if (draw_string) {
        
        if (large_font){
            eadk_display_push_rect_uniform((eadk_rect_t){point.x, point.y, 10 + 10 * index, 18}, eadk_color_white);
        }
        else{
            eadk_display_push_rect_uniform((eadk_rect_t){point.x, point.y, 7 * index, 14}, eadk_color_white);
        }
        eadk_display_draw_string(buffer, point, large_font, eadk_color_black, eadk_color_white);
        eadk_display_draw_string("Press exe to finish", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);
        eadk_display_draw_string("Press back to quit", (eadk_point_t){0, 220}, false, eadk_color_black, eadk_color_white);

        if (is_alpha) {
            eadk_display_push_rect_uniform((eadk_rect_t){240, 0, 80, 18}, eadk_color_white);
            eadk_display_draw_string("ALPHA", (eadk_point_t){270, 0}, large_font, eadk_color_black, eadk_color_white);
        }
        else {
            eadk_display_push_rect_uniform((eadk_rect_t){240, 0, 80, 18}, eadk_color_white);
            eadk_display_draw_string("NO ALPHA", (eadk_point_t){240, 0}, large_font, eadk_color_black, eadk_color_white);
        }
    }
    eadk_timing_msleep(300);
}

// Return the input of the user with point, draw_string and large_font in argument
char* input(eadk_point_t point, bool draw_string, bool large_font) {
    static char buffer[100] = ""; 
    size_t index = 0;
    bool is_alpha = false;

    memset(buffer, 0, sizeof(buffer));

    if (draw_string) {
        eadk_display_draw_string("NO ALPHA", (eadk_point_t){240, 0}, large_font, eadk_color_black, eadk_color_white);
    }

    while (eadk_keyboard_scan() != 0) {
        eadk_timing_msleep(100);
    }

    while (true) {
        eadk_keyboard_state_t state = eadk_keyboard_scan();

        if (eadk_keyboard_key_down(state, eadk_key_back)) {
            return ""; // Exit input on 'HOME' key press
        } 
        else if (eadk_keyboard_key_down(state, eadk_key_backspace)) {
            if (index > 0) {
                buffer[index - 1] = 0; // Remove the last character
                index--;
                refresh_input(buffer, index, point, is_alpha, draw_string, large_font);
            }
        } 
        else if (eadk_keyboard_key_down(state, eadk_key_alpha)) {
            is_alpha = !is_alpha; // Toggle alpha mode on 'ALPHA' key press
            refresh_input(buffer, index, point, is_alpha, draw_string, large_font);
        }
        else if (eadk_keyboard_key_down(state, eadk_key_exe)) {
            refresh_input(buffer, index, point, is_alpha, draw_string, large_font);
            break; // Return the buffer on 'EXE' key press
        } 
        else {
            for (size_t i = 0; i < sizeof(key_mappings) / sizeof(KeyMapping); i++) {
                if (eadk_keyboard_key_down(state, key_mappings[i].key)) {
                    char character = is_alpha ? key_mappings[i].alpha : key_mappings[i].normal;
                    if (character != 0) { // Only add character if it's not 0
                        buffer[index++] = character;
                        refresh_input(buffer, index, point, is_alpha, draw_string, large_font);
                    }
                    break;
                }
            }
        }
    }
    return buffer;
}

/*---------------------------------------------------------------------------*/

void refresh_input_int(const char *buffer, size_t index, eadk_point_t point, bool draw_string, bool large_font) {
    if (draw_string) {
        if (large_font){
            eadk_display_push_rect_uniform((eadk_rect_t){point.x, point.y, 10 + 10 * index, 18}, eadk_color_white);
        }
        else{
            eadk_display_push_rect_uniform((eadk_rect_t){point.x, point.y, 7 * index, 14}, eadk_color_white);
        }
        eadk_display_draw_string(buffer, point, large_font, eadk_color_black, eadk_color_white);
        eadk_display_draw_string("Press exe to finish", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);
        eadk_display_draw_string("Press back to quit", (eadk_point_t){0, 220}, false, eadk_color_black, eadk_color_white);
    }
    eadk_timing_msleep(300);
}

// Mapping of keys to characters
static const KeyMapping key_mappings_int[] = {
    {eadk_key_one, '1'},
    {eadk_key_two, '2'},
    {eadk_key_three, '3'},
    {eadk_key_four, '4'},
    {eadk_key_five, '5'},
    {eadk_key_six, '6'},
    {eadk_key_seven, '7'},
    {eadk_key_eight, '8'},
    {eadk_key_nine, '9'},
    {eadk_key_zero, '0'}
};

// Return the int input of the user with point, draw_string and large_font in argument
int input_int(eadk_point_t point, bool draw_string, bool large_font) {
    static char buffer[100] = ""; 
    size_t index = 0;

    memset(buffer, 0, sizeof(buffer));

    while (eadk_keyboard_scan() != 0) {
        eadk_timing_msleep(100);
    }

    while (true) {
        eadk_keyboard_state_t state = eadk_keyboard_scan();

        if (eadk_keyboard_key_down(state, eadk_key_back)) {
            return 0; // Exit input on 'back' key press
        } 
        else if (eadk_keyboard_key_down(state, eadk_key_backspace)) {
            if (index > 0) {
                buffer[index - 1] = 0; // Remove the last character
                index--;
                refresh_input_int(buffer, index, point, draw_string, large_font);
            }
        } 
        else if (eadk_keyboard_key_down(state, eadk_key_exe)) {
            refresh_input_int(buffer, index, point, draw_string, large_font);
            break; // Return the buffer on 'EXE' key press
        } 
        else {
            for (size_t i = 0; i < sizeof(key_mappings_int) / sizeof(KeyMapping); i++) {
                if (eadk_keyboard_key_down(state, key_mappings_int[i].key)) {
                    char character = key_mappings_int[i].normal;
                    if (character != 0) { // Only add character if it's not 0
                        buffer[index++] = character;
                        refresh_input_int(buffer, index, point, draw_string, large_font);
                    }
                    break;
                }
            }
        }
    }
    return atoi(buffer); // Convert the buffer to an integer and return it
}

/*---------------------------------------------------------------------------*/


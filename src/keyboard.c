#include "keyboard.h"
#include "libs/eadk.h"

typedef struct {
    eadk_key_t key;
    char normal;
    char shift;
    char alpha;
} KeyMapping;

static const KeyMapping key_mappings[] = {
    // {eadk_key_xnt, 0, 0, 0},
    // {eadk_key_alpha, 0, 0, 0},
    {eadk_key_xnt, 0, 0, ':'},
    {eadk_key_var, 0, 0, ';'},
    {eadk_key_toolbox, 0, 0, '"'},
    {eadk_key_backspace, '\b', 0, '%'},

    {eadk_key_exp, 0, '[', 'a'},
    {eadk_key_ln, 0, ']', 'b'},
    {eadk_key_log, 0, '{', 'c'},
    {eadk_key_imaginary, 0, '}', 'd'},
    {eadk_key_comma, ',', '_', 'e'},
    {eadk_key_power, 0, ',', 'f'},

    {eadk_key_sine, 0, 0, 'g'},
    {eadk_key_cosine, 0, 0, 'h'},
    {eadk_key_tangent, 0, 0, 'i'},
    {eadk_key_pi, 0, '=', 'j'},
    {eadk_key_sqrt, 0, '<', 'k'},
    {eadk_key_square, 0, '>', 'l'},

    {eadk_key_seven, '7', 0, 'm'},
    {eadk_key_eight, '8', 0, 'n'},
    {eadk_key_nine, '9', 0, 'o'},
    {eadk_key_left_parenthesis, '(', 0, 'p'},
    {eadk_key_right_parenthesis, ')', 0, 'q'},

    {eadk_key_four, '4', 0, 'r'},
    {eadk_key_five, '5', 0, 's'},
    {eadk_key_six, '6', 0, 't'},
    {eadk_key_multiplication, '*', 0, 'u'},
    {eadk_key_division, '/', 0, 'v'},

    {eadk_key_one, '1', 0, 'w'},
    {eadk_key_two, '2', 0, 'x'},
    {eadk_key_three, '3', 0, 'y'},
    {eadk_key_plus, '+', 0, 'z'},
    {eadk_key_minus, '-', 0, ' '},

    {eadk_key_zero, '0', 0, '?'},
    {eadk_key_dot, '.', 0, '!'}
    //{eadk_key_ee, 0, 0, 0},
    //{eadk_key_ans, 0, 0, 0},
    //{eadk_key_exe, '\n', 0, 0}
};

//return the char mapped to the key
char* input(void) {
    static char input_buffer[2] = {0, 0};

    for (size_t i = 0; i < sizeof(key_mappings) / sizeof(KeyMapping); i++) {
        if (eadk_keyboard_key_down(state, key_mappings[i].key)) {
            if (is_shift) {
                if (key_mappings[i].shift != 0) {
                    input_buffer[0] = key_mappings[i].shift;
                    return input_buffer;
                }
                return NULL;
            }

            if (is_alpha) {
                if (key_mappings[i].alpha != 0) {
                    input_buffer[0] = key_mappings[i].alpha;
                    return input_buffer;
                }
                return NULL;
            }

            if (key_mappings[i].normal != 0) {
                input_buffer[0] = key_mappings[i].normal;
                return input_buffer;
            }

            return NULL;
        }
    }

    return NULL;
}
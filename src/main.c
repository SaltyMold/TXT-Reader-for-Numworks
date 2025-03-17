#include "eadk.h"
#include <stdio.h>

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "TXT-READER";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

void draw_text(const char *text, eadk_point_t start_point, eadk_color_t fg, eadk_color_t bg) {
  eadk_point_t point = start_point;
  point.y += 16;
  const size_t max_line_length = 40; 
  char line[max_line_length + 1]; 
  size_t line_length = 0; 
  int line_number = 1; // Initialize line number

  while (*text) {
    size_t word_end = line_length; 
    while (line_length < max_line_length && *text && *text != '\n') {
      if (*text == ' ') {
        word_end = line_length;
      }
      line[line_length++] = *text++;
    }

    if (line_length == max_line_length && *text && *text != ' ' && *text != '\n') {
      if (word_end > 0) {
        text -= (line_length - word_end); 
        line_length = word_end; 
      }
    }

    line[line_length] = '\0'; 
    eadk_display_draw_string(line, point, false, fg, bg);

    // Draw line number to the right
    char line_number_str[10];
    snprintf(line_number_str, sizeof(line_number_str), "%d", line_number);
    eadk_display_draw_string(line_number_str, (eadk_point_t){EADK_SCREEN_WIDTH - 21, point.y}, false, fg, bg);

    point.y += 16; 
    line_length = 0; 
    line_number++; // Increment line number

    while (*text == ' ') {
      text++;
    }

    if (*text == '\n') {
      text++;
    }
  }

  if (line_length > 0) {
    line[line_length] = '\0';
    eadk_display_draw_string(line, point, false, fg, bg);

    // Draw line number to the right
    char line_number_str[10];
    snprintf(line_number_str, sizeof(line_number_str), "%d", line_number);
    eadk_display_draw_string(line_number_str, (eadk_point_t){EADK_SCREEN_WIDTH - 30, point.y}, false, fg, bg);
  }
}

int main(int argc, char * argv[]) {
  eadk_rect_t full_screen_rect = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white); 

  int scroll_offset = 0; 
  bool refresh_screen = true;

  while (true) {
    eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_home)) {
      break; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_down)) {
      scroll_offset += 16; 
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_up)) {
      scroll_offset -= 16;
      if (scroll_offset < 0) {
        scroll_offset = 0; 
      }
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_right)) {
      scroll_offset += 160; // Advance by 10 lines
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_left)) {
      scroll_offset -= 160; // Go back by 10 lines
      if (scroll_offset < 0) {
        scroll_offset = 0; 
      }
      refresh_screen = true; 
    }

    if (refresh_screen) {
      eadk_display_push_rect_uniform((eadk_rect_t){0, 18, 320, 222}, eadk_color_white); 

      if (eadk_external_data != NULL && eadk_external_data_size > 0) {
        draw_text(eadk_external_data, (eadk_point_t){0, -scroll_offset}, eadk_color_black, eadk_color_white);

        eadk_rect_t titre = {0, 0, 320, 18};
        eadk_display_push_rect_uniform(titre, eadk_color_orange);
      
        eadk_display_draw_string("TXT-READER", (eadk_point_t){125, 3}, false, eadk_color_white, eadk_color_orange);
        
      } else {
        eadk_display_draw_string("No external data", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
      }
      refresh_screen = false; 
    }

    eadk_timing_msleep(100); 
  }

  return 0; 
}

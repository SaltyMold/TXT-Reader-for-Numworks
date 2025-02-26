#include "eadk.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "TXT_READER";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

void draw_text(const char *text, eadk_point_t start_point, eadk_color_t fg, eadk_color_t bg) {
  eadk_point_t point = start_point;
  const size_t max_line_length = 33; 
  char line[max_line_length + 1]; 
  size_t line_length = 0; 

  while (*text) {
    size_t word_end = line_length; 
    while (line_length < max_line_length && *text && *text != '\n') {
      if (*text == ' ') {
        word_end = line_length;
      }
      line[line_length++] = *text++;
    }

    // Si la ligne est trop longue, revenir au dernier espace
    if (line_length == max_line_length && *text && *text != ' ' && *text != '\n') {
      if (word_end > 0) {
        text -= (line_length - word_end); 
        line_length = word_end; 
      }
    }

    line[line_length] = '\0'; 
    eadk_display_draw_string(line, point, true, fg, bg);
    point.y += 16; 
    line_length = 0; 

    while (*text == ' ') {
      text++;
    }

    if (*text == '\n') {
      text++;
    }
  }

  if (line_length > 0) {
    line[line_length] = '\0';
    eadk_display_draw_string(line, point, true, fg, bg);
  }
}

int main(int argc, char * argv[]) {
  eadk_rect_t full_screen_rect = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white); 

  int scroll_offset = 0; 
  bool refresh_screen = true;

  while (true) {
    eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_back)) {
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

    if (refresh_screen) {
      eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white); 
      if (eadk_external_data != NULL && eadk_external_data_size > 0) {
        draw_text(eadk_external_data, (eadk_point_t){0, -scroll_offset}, eadk_color_black, eadk_color_white);
      } else {
        eadk_display_draw_string("No external data", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
      }
      refresh_screen = false; 
    }

    eadk_timing_msleep(100); 
  }

  return 0; // Terminer le programme
}

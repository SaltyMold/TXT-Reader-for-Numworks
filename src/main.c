#include "eadk.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "TXT-READER";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;


int scroll_offset = 0;


/*------------------------------------------------------------------------*/

void draw_text(const char *text, eadk_point_t start_point, eadk_color_t fg, eadk_color_t bg) {
  eadk_point_t point = start_point;
  point.y += 16;
  const size_t max_line_length = 40; 
  char line[max_line_length + 1]; 
  size_t line_length = 0; 
  int line_number = 1; 

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

    char line_number_str[10];
    snprintf(line_number_str, sizeof(line_number_str), "%d", line_number);
    eadk_display_draw_string(line_number_str, (eadk_point_t){EADK_SCREEN_WIDTH - 21, point.y}, false, fg, bg);

    point.y += 16; 
    line_length = 0; 
    line_number++; 

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

    char line_number_str[10];
    snprintf(line_number_str, sizeof(line_number_str), "%d", line_number);
    eadk_display_draw_string(line_number_str, (eadk_point_t){EADK_SCREEN_WIDTH - 30, point.y}, false, fg, bg);
  }
}

/*---------------------------------------------------------------------------*/

bool* search_in_external_data(const char* search_string, size_t* line_count) {
  if (eadk_external_data == NULL || eadk_external_data_size == 0 || search_string == NULL || strlen(search_string) == 0) {
      *line_count = 0;
      return NULL;
  }

  const char* data = eadk_external_data;
  *line_count = 1;
  for (size_t i = 0; i < eadk_external_data_size; i++) {
      if (data[i] == '\n') {
          (*line_count)++;
      }
  }

  bool* apparitions = (bool*)calloc(*line_count, sizeof(bool)); 
  if (apparitions == NULL) {
      *line_count = 0;
      return NULL;
  }

  size_t current_line = 0;
  const char* line_start = data;
  for (size_t i = 0; i <= eadk_external_data_size; i++) {
      if (data[i] == '\n' || data[i] == '\0') {
          size_t line_length = data + i - line_start;
          char* line = (char*)malloc(line_length + 1);
          if (line == NULL) {
              free(apparitions);
              *line_count = 0;
              return NULL;
          }
          strncpy(line, line_start, line_length);
          line[line_length] = '\0';

          if (strstr(line, search_string) != NULL) {
              apparitions[current_line] = true;
          }

          free(line);
          line_start = data + i + 1;
          current_line++;
      }
  }

  return apparitions;
}

void search() {
  eadk_display_push_rect_uniform((eadk_rect_t){0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT}, eadk_color_white);
  eadk_display_draw_string("Search :", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press exe to finish", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press back to quit", (eadk_point_t){0, 220}, false, eadk_color_black, eadk_color_white);

  char* search_string = input((eadk_point_t){0, 80}, true, true);
  if (strlen(search_string) == 0) {
      eadk_display_draw_string("Empty search string.", (eadk_point_t){0, 100}, false, eadk_color_black, eadk_color_white);
      eadk_timing_msleep(1000);
      return;
  }

  size_t line_count = 0;
  bool* apparitions = search_in_external_data(search_string, &line_count);

  if (apparitions != NULL) {
      const int lines_per_column = 10; 
      const int columns_per_page = 5;
      const int results_per_page = lines_per_column * columns_per_page; 
      int current_page = 0;

      size_t total_results = 0;
      for (size_t i = 0; i < line_count; i++) {
          if (apparitions[i]) {
              total_results++;
          }
      }

      int total_pages = (total_results + results_per_page - 1) / results_per_page;

      size_t* result_indices = (size_t*)malloc(total_results * sizeof(size_t));
      if (result_indices == NULL) {
          free(apparitions);
          return;
      }

      size_t result_index = 0;
      for (size_t i = 0; i < line_count; i++) {
          if (apparitions[i]) {
              result_indices[result_index++] = i;
          }
      }

      while (true) {
          eadk_display_push_rect_uniform((eadk_rect_t){0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT}, eadk_color_white);
          eadk_display_draw_string("Results:", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
          eadk_display_draw_string("Press left/right to change page", (eadk_point_t){0, 25}, false, eadk_color_black, eadk_color_white);
          eadk_display_draw_string("Press back to quit search", (eadk_point_t){0, 40}, false, eadk_color_black, eadk_color_white);

          char page_info[30];
          snprintf(page_info, sizeof(page_info), "Page %d/%d", current_page + 1, total_pages);
          eadk_display_draw_string(page_info, (eadk_point_t){EADK_SCREEN_WIDTH - 65, 0}, false, eadk_color_black, eadk_color_white);

          char result[50];
          int displayed_lines = 0;

          for (size_t i = current_page * results_per_page; i < total_results && displayed_lines < results_per_page; i++) {
              int column = displayed_lines / lines_per_column;
              int row = displayed_lines % lines_per_column;

              int x_position = column * 63;
              int y_position = 75 + row * 16; 

              snprintf(result, sizeof(result), "Line %d", (int)(result_indices[i] + 1));
              eadk_display_draw_string(result, (eadk_point_t){x_position, y_position}, false, eadk_color_black, eadk_color_white);

              displayed_lines++;
          }

          while (true) {
              eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
              if (eadk_keyboard_key_down(keyboard_state, eadk_key_back) || eadk_keyboard_key_down(keyboard_state, eadk_key_home)) {
                  free(apparitions);
                  free(result_indices);
                  return; 
              }
              if (eadk_keyboard_key_down(keyboard_state, eadk_key_left)) {
                  if (current_page > 0) {
                      current_page--; 
                      eadk_timing_msleep(300);
                  }
                  break;
              }
              if (eadk_keyboard_key_down(keyboard_state, eadk_key_right)) {
                  if (current_page < total_pages - 1) {
                      current_page++; 
                      eadk_timing_msleep(300);
                  }
                  break;
              }
              eadk_timing_msleep(100); 
          }
      }
  } else {
      eadk_display_draw_string("Error or no results.", (eadk_point_t){0, 75}, false, eadk_color_black, eadk_color_white);
      eadk_timing_msleep(2000);
  }
}

/*---------------------------------------------------------------------------*/

void go_to(){
  eadk_display_push_rect_uniform((eadk_rect_t){0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT}, eadk_color_white);
  eadk_display_draw_string("Go to line n:", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press exe to finish", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press back to quit", (eadk_point_t){0, 220}, false, eadk_color_black, eadk_color_white);

  int line_number = input_int((eadk_point_t){0, 30}, true, true);
  if (line_number == 0) {
      return;
  }

  scroll_offset = (line_number - 1) * 16; 
  
  draw_text(eadk_external_data, (eadk_point_t){0, -scroll_offset}, eadk_color_black, eadk_color_white);

}

/*-------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  eadk_rect_t full_screen_rect = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white);
  eadk_display_draw_string("Loading...", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
  eadk_timing_msleep(500);
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white);

  eadk_display_draw_string("The calculator has issues with third-party\napplications. Save your Python script before\nlaunching the app to avoid data loss. If it\ncrashes, reinstall the app.", (eadk_point_t){0, 0}, false, eadk_color_orange, eadk_color_white);


  eadk_display_draw_string("Press home to quit", (eadk_point_t){0, 80}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press shift to search", (eadk_point_t){0, 100}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press alpha to go to line n", (eadk_point_t){0, 120}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press up/down to scroll", (eadk_point_t){0, 140}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press left/right to scroll 10 lines", (eadk_point_t){0, 160}, false, eadk_color_black, eadk_color_white);

  eadk_display_draw_string("Press OK to start", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);

  while (true){
    eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_ok) || eadk_keyboard_key_down(keyboard_state, eadk_key_home)) {
      eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white);
      break;
    }
  }

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
      scroll_offset += 160; 
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_left)) {
      scroll_offset -= 160;
      if (scroll_offset < 0) {
        scroll_offset = 0; 
      }
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_shift)) {
      search(); 
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_alpha)) {
      go_to(); 
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

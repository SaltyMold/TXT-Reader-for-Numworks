#include "libs/eadk.h"
#include "keyboard.h"
#include "periodic.h"

#define SIMULATOR 0
// No storage for simulator
// Simulator need keyboard scan to work

#if !SIMULATOR 
#include "libs/storage.h"
#endif

#define DEBUG 0

#define PERIODIC 1 // Hidden in periodic table

#define SVG_FILE "txt-reader.svg"

#if PERIODIC
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Periodic";
#else
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "TXT-Reader";
#endif
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

void safe_sleep(int ms) {
	#if SIMULATOR
	eadk_keyboard_scan();
	#endif
	for (int i = 0; i < ms; i += 10) {
		eadk_timing_msleep(10);
	}
}

typedef struct {
    const char *data;
    bool first_line;
} line_t;

line_t *split_lines(const char *txt, int *out_count) {
    int capacity = 16;
    int count = 0;
    line_t *result = malloc(capacity * sizeof(line_t));
    if (!result) return NULL;

    const char *p = txt;
    const char *line_start = p;

    while (*p) {
        int line_len = p - line_start;

        if (*p == '\n' || line_len >= 40) {
            int len;

            if (*p == '\n') {
                len = p - line_start;
            } else {
                const char *space = line_start + 40;
                while (space > line_start && *space != ' ' && *space != '\t') {
                    space--;
                }
                if (space == line_start) {
                    len = 40;
                } else {
                    len = space - line_start;
                }
            }

            char *chunk = malloc(len + 1);
            memcpy(chunk, line_start, len);
            chunk[len] = '\0';

            if (count >= capacity) {
                capacity *= 2;
                result = realloc(result, capacity * sizeof(line_t));
            }

            result[count].data = chunk;
            result[count].first_line = (line_start == txt) || (*(line_start - 1) == '\n');
            count++;

            if (*p == '\n') {
                line_start = p + 1;
            } else {
                line_start += len;
                while (*line_start == ' ' || *line_start == '\t') line_start++;
                p = line_start - 1;
            }
        }
        p++;
    }

    if (line_start < p) {
        int len = p - line_start;
        char *chunk = malloc(len + 1);
        memcpy(chunk, line_start, len);
        chunk[len] = '\0';

        if (count >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(line_t));
        }

        result[count].data = chunk;
        result[count].first_line = (line_start == txt) || (*(line_start - 1) == '\n');
        count++;
    }

    *out_count = count;
    return result;
}

int logical_line_number(line_t *lines, int index) {
    int n = 1;
    for (int i = 0; i < index; i++) {
        if (lines[i].first_line)
            n++;
    }
    return n;
}


int find_all_in_array(line_t lines[], int arr_count, const char *txt_to_find, int *out_indexes, int max_indexes){
    int found = 0;

    if (!txt_to_find || !*txt_to_find)
        return 0;

    for (int i = 0; i < arr_count; i++) {

		if (!lines[i].data)
			continue;

		const char *h = lines[i].data;
		const char *n = txt_to_find;

		for (; *h; h++) {
			const char *hh = h;
			const char *nn = n;

			while (*hh && *nn &&
				   tolower((unsigned char)*hh) ==
				   tolower((unsigned char)*nn)) {
				hh++;
				nn++;
			}

			if (!*nn) {
				int start = i;
				while (start > 0 && !lines[start].first_line) start--;

				bool duplicate = false;
				for (int k = 0; k < found; k++) {
					if (out_indexes[k] == start) { duplicate = true; break; }
				}

				if (!duplicate) {
					if (found < max_indexes) {
						out_indexes[found] = start;
						found++;
					} else {
						return found;
					}
				}
				break;
			}
		}
    }

    return found;
}

void menu() {
	eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);
	eadk_display_draw_string("This screen will\nno longer show up.", (eadk_point_t){0, 0}, true, eadk_color_red, eadk_color_black);
	eadk_display_draw_string("To access the txt reader you need to\ngo on the Carbon element\nand press the key \"9\" 5 times", (eadk_point_t){0, 40}, false, eadk_color_white, eadk_color_black);
	eadk_display_draw_string("Use up/down to scroll", (eadk_point_t){0, 100}, false, eadk_color_white, eadk_color_black);
	eadk_display_draw_string("Use left/right to scroll 10 lines", (eadk_point_t){0, 115}, false, eadk_color_white, eadk_color_black);
	eadk_display_draw_string("Search by typing.", (eadk_point_t){0, 130}, false, eadk_color_white, eadk_color_black);
	eadk_display_draw_string("Use Ans and EXE to scroll searched data", (eadk_point_t){0, 145}, false, eadk_color_white, eadk_color_black);
	eadk_display_draw_string("Press BACK to quit the app.", (eadk_point_t){0, 160}, false, eadk_color_white, eadk_color_black);
	eadk_display_draw_string("Press OK to continue", (eadk_point_t){0, 228}, false, eadk_color_white, eadk_color_black);

	safe_sleep(5000);
	while (!eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_ok) && !eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_back));
}

void display_lines(line_t *lines, int nb_lines, int line_index, int highlight_index) {
	int number = 1;
	for (int j = 0; j < line_index; j++) {
		if (lines[j].first_line) number++;
	}

	char line_number[8];
	for (int i = 0; (nb_lines > 14) ? i < 14 : i < nb_lines; i++){
		int idx = i + line_index;
		// Data
		eadk_display_draw_string(lines[idx].data, (eadk_point_t){2, 22 + i * 14}, false, eadk_color_black, eadk_color_white);
        
		// Line number
		if (lines[idx].first_line) {
			snprintf(line_number, sizeof(line_number), "%4d", number);
			eadk_display_draw_string(line_number, (eadk_point_t){290, 22 + i * 14}, false, eadk_color_black, (highlight_index == idx) ? eadk_color_orange : eadk_color_white);
			number++;
		}
	}
}

void display_text(line_t *lines, int nb_lines, int line_index, int highlight_line) {
	eadk_display_push_rect_uniform((eadk_rect_t){0, 18, 320, 200}, eadk_color_white);
	eadk_display_push_rect_uniform((eadk_rect_t){285, 20, 1, 198}, eadk_color_black);
	display_lines(lines, nb_lines, line_index, highlight_line);
}

void display_bottom(){
	eadk_display_push_rect_uniform((eadk_rect_t){0, 218, 320, 1}, eadk_color_black);
	eadk_display_push_rect_uniform((eadk_rect_t){2, 221, 112, 18}, eadk_color_black);
	eadk_display_push_rect_uniform((eadk_rect_t){3, 222, 110, 16}, eadk_color_white);
}

bool is_shift;
bool is_alpha;

eadk_keyboard_state_t state;

int main(int argc, char * argv[]) {
	#if SIMULATOR
	//menu();
	#else 
	if (extapp_fileExists(SVG_FILE)) {
		
	}
	else {
		menu();
		extapp_fileWrite(SVG_FILE, "", 1);
	}	
	#endif

	#if PERIODIC
	periodic();
	#endif
	
	int nb_lines;
	line_t *lines = split_lines(eadk_external_data, &nb_lines);

	int line_index = 0;

	char input_buffer[16] = {0};

	int found_indexes[256];
	int found_count = 0;
	int search_index = 0;

	char text_buffer[32];

	bool is_search = false;

	eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
	eadk_display_push_rect_uniform((eadk_rect_t){0, 0, 320, 18}, eadk_color_orange);
	eadk_display_draw_string("TXT-READER", (eadk_point_t){125, 3}, false, eadk_color_white, eadk_color_orange);

	display_text(lines, nb_lines, line_index, -1);
	display_bottom();
	eadk_display_draw_string("Search by typing", (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);

	int true_nb_lines = 0;
	for (int i = 0; i < nb_lines; i++){
		if (lines[i].first_line) true_nb_lines++;
	}
	char true_nb_lines_buf[8];
	snprintf(true_nb_lines_buf, sizeof(true_nb_lines_buf), "/%4d", true_nb_lines);
	eadk_display_draw_string(true_nb_lines_buf, (eadk_point_t){283, 224}, false, eadk_color_black, eadk_color_white);

	while (1) {
		state = eadk_keyboard_scan();

		if (eadk_keyboard_key_down(state, eadk_key_back)) break;

		if (eadk_keyboard_key_down(state, eadk_key_shift)) {
			is_shift = !is_shift;
			is_alpha = false;
			if (is_shift) eadk_display_draw_string("shift", (eadk_point_t){280, 3}, false, eadk_color_white, eadk_color_orange);
			else eadk_display_push_rect_uniform((eadk_rect_t){280, 0, 40, 18}, eadk_color_orange);
			while (eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_shift));
		}
		else if (eadk_keyboard_key_down(state, eadk_key_alpha)) {
			is_alpha = !is_alpha;
			is_shift = false;
			if (is_alpha) eadk_display_draw_string("alpha", (eadk_point_t){280, 3}, false, eadk_color_white, eadk_color_orange);
			else eadk_display_push_rect_uniform((eadk_rect_t){280, 0, 40, 18}, eadk_color_orange);
			while (eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_alpha));
		}
		
		if (eadk_keyboard_key_down(state, eadk_key_down) && (line_index + 14 < nb_lines)) {
			line_index += 1;
			display_text(lines, nb_lines, line_index, (is_search) ? found_indexes[search_index] : -1);
			safe_sleep(100);
		}
		else if (eadk_keyboard_key_down(state, eadk_key_up) && (line_index > 0)) {
			line_index -= 1;
			display_text(lines, nb_lines, line_index, (is_search) ? found_indexes[search_index] : -1);
			safe_sleep(100);
		}
		else if (eadk_keyboard_key_down(state, eadk_key_right)) {
			line_index = (line_index + 10 + 14 < nb_lines) ? line_index + 10 : (nb_lines > 14) ? (nb_lines - 14) : 0;
			display_text(lines, nb_lines, line_index, (is_search) ? found_indexes[search_index] : -1);
			safe_sleep(100);
		}
		else if (eadk_keyboard_key_down(state, eadk_key_left)) {
			line_index = (line_index - 10 > 0) ? line_index - 10 : 0;
			display_text(lines, nb_lines, line_index, (is_search) ? found_indexes[search_index] : -1);
			safe_sleep(100);
		}
		else {
			char* result = input();
			if (result) {
				if (result[0] == '\b') {
					size_t len = strlen(input_buffer);
					if (len > 0) input_buffer[len - 1] = '\0';
					display_bottom();
				} else {
					strncat(input_buffer, result, sizeof(input_buffer) - strlen(input_buffer) - 1);
				}
				eadk_display_draw_string(input_buffer, (eadk_point_t){5, 224}, false, eadk_color_black, eadk_color_white);

				if (input_buffer[0] != '\0') {
					is_search = true;
					found_count = find_all_in_array(lines, nb_lines, input_buffer, found_indexes, (int)(sizeof(found_indexes)/sizeof(found_indexes[0])));
					if (!found_count) {
						eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
						eadk_display_draw_string("No results found", (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
						is_search = false;
					} else {
						snprintf(text_buffer, sizeof(text_buffer), "%d of %d -> line %d", search_index + 1, found_count, logical_line_number(lines, found_indexes[search_index]));
						eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
						eadk_display_draw_string(text_buffer, (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
					}
				}	
				else if (is_search){
					eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
					eadk_display_draw_string("Search by typing", (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
					is_search = false;
				}
				search_index = 0;
				display_text(lines, nb_lines, line_index, (is_search) ? found_indexes[search_index] : -1);
			}
			while (eadk_keyboard_scan() == state);
		}

		if (eadk_keyboard_key_down(state, eadk_key_ans) && is_search) {
			if (search_index > 0) search_index--;
			else search_index = found_count - 1;
			snprintf(text_buffer, sizeof(text_buffer), "%d of %d -> line %d", search_index + 1, found_count, logical_line_number(lines, found_indexes[search_index]));
			eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
			eadk_display_draw_string(text_buffer, (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);

			line_index = (found_indexes[search_index] > 0) ? (found_indexes[search_index] + 14 < nb_lines ? found_indexes[search_index] : (nb_lines > 14 ? nb_lines - 14 : 0)) : 0;
			display_text(lines, nb_lines, line_index, found_indexes[search_index]);

			safe_sleep(100);
		}
		else if (eadk_keyboard_key_down(state, eadk_key_exe) && is_search) {
			if (search_index < found_count - 1) search_index++;
			else search_index = 0;
			snprintf(text_buffer, sizeof(text_buffer), "%d of %d -> line %d", search_index + 1, found_count, logical_line_number(lines, found_indexes[search_index]));
			eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
			eadk_display_draw_string(text_buffer, (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);

			line_index = (found_indexes[search_index] + 14 < nb_lines) ? found_indexes[search_index] : (nb_lines > 14) ? nb_lines - 14 : 0;
			display_text(lines, nb_lines, line_index, found_indexes[search_index]);

			safe_sleep(100);
		}

		#if DEBUG
		char debug_msg[64];
		snprintf(debug_msg, sizeof(debug_msg), "found_count: %d", found_count);
		eadk_display_draw_string(debug_msg, (eadk_point_t){0, 0}, false, eadk_color_white, eadk_color_black);
		#endif
	}

	return 0;
}
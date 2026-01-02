#include "libs/eadk.h"
#include "keyboard.h"

#define SIMULATOR 0
// No storage for simulator
// Simulator need keyboard scan to work

#define PERIODIC 1
// Hidden in periodic table

#define SVG_FILE "txt-reader.svg"

#if !SIMULATOR 
#include "libs/storage.h"
#endif

#if PERIODIC
#include "periodic.h"
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Periodic";
#else
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "TXT-Reader";
#endif

const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

extern const char* eadk_external_data;
extern size_t eadk_external_data_size;


#define MAX_LINE_LEN 40
#define MAX_VISIBLE_LINES 14
#define SCROLL_V_STEP 1
#define MAX_FOUND 1024

typedef struct {
    const char* start;
    int len;
    int logical_line_number;
    bool is_first_wrap;
} view_line_t;

bool is_shift = false;
bool is_alpha = false;
eadk_keyboard_state_t state;


void display_view_by_line(int line_number, int highlight_line) {
    view_line_t lines[MAX_VISIBLE_LINES];
    int nb_lines = 0;
    int i = 0;
    int current_logical_line = 0;

    while (i < (int)eadk_external_data_size && current_logical_line < line_number) {
        if (eadk_external_data[i] == '\n') current_logical_line++;
        i++;
    }

    bool first_subline = true;

    while (i < (int)eadk_external_data_size && nb_lines < MAX_VISIBLE_LINES) {
        const char* line_start = eadk_external_data + i;
        int len = 0;

        while (i + len < (int)eadk_external_data_size &&
               eadk_external_data[i + len] != '\n' &&
               len < MAX_LINE_LEN) {
            len++;
        }

        if (len == MAX_LINE_LEN &&
            i + len < (int)eadk_external_data_size &&
            eadk_external_data[i + len] != '\n') {
            int back = len - 1;
            while (back > 0 && !isspace((unsigned char)eadk_external_data[i + back])) back--;
            if (back > 0) len = back;
        }

        lines[nb_lines].start = line_start;
        lines[nb_lines].len = len;
        lines[nb_lines].logical_line_number = current_logical_line + 1;
        lines[nb_lines].is_first_wrap = first_subline;
        first_subline = false;
        nb_lines++;

        i += len;
        while (i < (int)eadk_external_data_size &&
               (eadk_external_data[i] == ' ' || eadk_external_data[i] == '\t')) i++;

        if (i < (int)eadk_external_data_size && eadk_external_data[i] == '\n') {
            i++;
            current_logical_line++;
            first_subline = true;
        }
    }

    eadk_display_push_rect_uniform((eadk_rect_t){0,18,320,200}, eadk_color_white);
	eadk_display_push_rect_uniform((eadk_rect_t){285, 20, 1, 198}, eadk_color_black);

    char tmp[MAX_LINE_LEN + 1];
    char num_buf[8];
    for (int j = 0; j < nb_lines; j++) {
        memcpy(tmp, lines[j].start, lines[j].len);
        tmp[lines[j].len] = '\0';

        if (lines[j].is_first_wrap) {
            snprintf(num_buf, sizeof(num_buf), "%4d", lines[j].logical_line_number);
            eadk_display_draw_string(num_buf, (eadk_point_t){290, 22 + j * 14},
                                     false, eadk_color_black,
                                     (highlight_line == lines[j].logical_line_number) ? eadk_color_orange : eadk_color_white);
        }

        eadk_display_draw_string(tmp, (eadk_point_t){2, 22 + j * 14},
                                 false, eadk_color_black,
                                 (highlight_line == lines[j].logical_line_number) ? eadk_color_orange : eadk_color_white);
    }
}

int count_lines_in_external_data(void) {
	int count = 0;
	for (size_t i = 0; i < eadk_external_data_size; i++) {
		if (eadk_external_data[i] == '\n') count++;
	}
	if (eadk_external_data_size > 0 &&
		eadk_external_data[eadk_external_data_size - 1] != '\n') {
		count++;
	}
	return count;
}

void display_init(int nb_lines) {
    eadk_display_push_rect_uniform((eadk_rect_t){0, 0, 320, 240}, eadk_color_white);
    eadk_display_push_rect_uniform((eadk_rect_t){0,0,320,18}, eadk_color_orange);
    eadk_display_draw_string("TXT-READER", (eadk_point_t){125,3}, false, eadk_color_white, eadk_color_orange);

    eadk_display_push_rect_uniform((eadk_rect_t){285, 20, 1, 198}, eadk_color_black);

    eadk_display_push_rect_uniform((eadk_rect_t){0, 218, 320, 1}, eadk_color_black);
    eadk_display_push_rect_uniform((eadk_rect_t){2, 221, 112, 18}, eadk_color_black);
    eadk_display_push_rect_uniform((eadk_rect_t){3, 222, 110, 16}, eadk_color_white);

	eadk_display_draw_string("Search by typing", (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
	
	char nb_lines_buf[8];
	snprintf(nb_lines_buf, sizeof(nb_lines_buf), "/%4d", nb_lines);
	eadk_display_draw_string(nb_lines_buf, (eadk_point_t){283, 224}, false, eadk_color_black, eadk_color_white);
}

int search_in_external_data(const char* word, int* found_indexes) {
    int found_count = 0;
    int line_number = 0;
    int i = 0;

    if (!word || !*word) return 0;

    while (i < (int)eadk_external_data_size) {
        const char* line_start = eadk_external_data + i;
        int len = 0;

        while (i + len < (int)eadk_external_data_size && eadk_external_data[i + len] != '\n') len++;

        for (int j = 0; j <= len - (int)strlen(word); j++) {
            int k;
            for (k = 0; k < (int)strlen(word); k++) {
                if (tolower((unsigned char)line_start[j + k]) != tolower((unsigned char)word[k]))
                    break;
            }
            if (k == (int)strlen(word)) {
                if (found_count < MAX_FOUND)
                    found_indexes[found_count++] = line_number;
                break;
            }
        }

        i += len;
        if (i < (int)eadk_external_data_size && eadk_external_data[i] == '\n') i++;
        line_number++;
    }

    return found_count;
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

	for (int i = 0; i < 500; i++) eadk_timing_msleep(10);
	
	while (!eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_ok) && !eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_back));
}

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

	int nb_lines = count_lines_in_external_data();

    int scroll_line = 0;

    char input_buffer[16] = {0};
    bool is_search = false;
    int found_indexes[MAX_FOUND] = {0};
    int found_count = 0;
    int search_index = 0;

    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
    display_init(nb_lines);
    display_view_by_line(scroll_line, -1);

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

        if (eadk_keyboard_key_down(state, eadk_key_down)) {
            scroll_line += SCROLL_V_STEP;
			if (scroll_line + MAX_VISIBLE_LINES - 2 > nb_lines) scroll_line = nb_lines - MAX_VISIBLE_LINES + 2;
            display_view_by_line(scroll_line, is_search && found_count > 0 ? found_indexes[search_index] + 1 : -1);
            eadk_timing_msleep(100);
        }
        if (eadk_keyboard_key_down(state, eadk_key_up)) {
            scroll_line -= SCROLL_V_STEP;
            if (scroll_line < 0) scroll_line = 0;
            display_view_by_line(scroll_line, is_search && found_count > 0 ? found_indexes[search_index] + 1 : -1);
            eadk_timing_msleep(100);
        }
        if (eadk_keyboard_key_down(state, eadk_key_right)) {
            scroll_line += 10;
			if (scroll_line + MAX_VISIBLE_LINES - 2 > nb_lines) scroll_line = nb_lines - MAX_VISIBLE_LINES + 2;
            display_view_by_line(scroll_line, is_search && found_count > 0 ? found_indexes[search_index] + 1 : -1);
            eadk_timing_msleep(100);
        }
		if (eadk_keyboard_key_down(state, eadk_key_left)) {
			scroll_line -= 10;
			if (scroll_line < 0) scroll_line = 0;
			display_view_by_line(scroll_line, is_search && found_count > 0 ? found_indexes[search_index] + 1 : -1);
			eadk_timing_msleep(100);
		}

		if (eadk_keyboard_key_down(state, eadk_key_ans) && is_search) {
			if (search_index > 0) search_index--;
			else search_index = found_count - 1;
		
			eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
			char text_buffer[64];
			snprintf(text_buffer, sizeof(text_buffer), "%d of %d -> line %d", search_index + 1, found_count, found_indexes[search_index] + 1);
			eadk_display_draw_string(text_buffer, (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
			display_view_by_line(scroll_line, found_indexes[search_index] + 1);

			scroll_line = found_indexes[search_index]; 
			display_view_by_line(scroll_line, found_indexes[search_index] + 1);

			while (eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_ans));
		}

		if (eadk_keyboard_key_down(state, eadk_key_exe) && is_search) {
			if (search_index < found_count - 1) search_index++;
			else search_index = 0;
		
			eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
			char text_buffer[64];
			snprintf(text_buffer, sizeof(text_buffer), "%d of %d -> line %d", search_index + 1, found_count, found_indexes[search_index] + 1);
			eadk_display_draw_string(text_buffer, (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
			display_view_by_line(scroll_line, found_indexes[search_index] + 1);

			scroll_line = found_indexes[search_index]; 
			display_view_by_line(scroll_line, found_indexes[search_index] + 1);

			while (eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_exe));
		}

        char* result = input();
        if (result) {
            if (result[0] == '\b') {
                size_t len = strlen(input_buffer);
                if (len > 0) input_buffer[len - 1] = '\0';
            } else {
                strncat(input_buffer, result, sizeof(input_buffer) - strlen(input_buffer) - 1);
            }

            if (input_buffer[0] != '\0') {
                is_search = true;
                found_count = search_in_external_data(input_buffer, found_indexes);
                search_index = 0;
            } else {
                is_search = false;
                found_count = 0;
                search_index = 0;
            }

			eadk_display_push_rect_uniform((eadk_rect_t){3, 222, 110, 16}, eadk_color_white);
			eadk_display_draw_string(input_buffer, (eadk_point_t){5, 224}, false, eadk_color_black, eadk_color_white);
			
			if (!is_search){
				eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
				eadk_display_draw_string("Search by typing", (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
				display_view_by_line(scroll_line, -1);
			}
			else if (is_search && found_count > 0) {
				eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
				char text_buffer[64];
				snprintf(text_buffer, sizeof(text_buffer), "%d of %d -> line %d", search_index + 1, found_count, found_indexes[search_index] + 1);
				eadk_display_draw_string(text_buffer, (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
				display_view_by_line(scroll_line, found_indexes[search_index] + 1);
			}
			else {
				eadk_display_push_rect_uniform((eadk_rect_t){120, 224, 160, 16}, eadk_color_white);
				eadk_display_draw_string("No results found", (eadk_point_t){120, 224}, false, eadk_color_black, eadk_color_white);
				display_view_by_line(scroll_line, -1);
			}

			while (eadk_keyboard_scan() == state);
        }

    }

    return 0;
}

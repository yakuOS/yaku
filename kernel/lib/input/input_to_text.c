#include "input_to_text.h"

#include <lib/input/read_input.h>
#include <lib/string.h>
#include <resources/keyboard_stringmap.h>
#include <string.h>
#include <types.h>

void (*input_to_text_on_enter_callback)(char*);
void (*input_to_text_on_string_change_callback)(char*);
void (*input_to_text_on_keypress_callback)(char*);
void (*input_to_text_on_arrow_up_callback)(char*);
void (*input_to_text_on_arrow_down_callback)(char*);

bool input_to_text_enter_callback = false;
bool input_to_text_string_change_callback_callback = false;
bool input_to_text_keypress_callback = false;
bool input_to_text_arrow_up_callback = false;
bool input_to_text_arrow_down_callback = false;

char* input_to_text_field[1012];

read_input_listener input_to_text_listener;

bool input_to_text_shift_pressed = false;
bool input_to_text_caps_pressed = false;
bool input_to_text_altgr_pressed = false;

bool function_key = false; // next key is a key like: alt_gr, delete, and other keys that
                           // send multiple bytes
uint8_t keycount = 0;

uint32_t cursor = 0;

void input_to_text_handle_keystroke(char keystroke, read_input_listener* this) {
    if (keystroke <= 0) {
        if (keystroke == -74) {
            input_to_text_shift_pressed = false;
        }
        if (keystroke == -86) {
            input_to_text_shift_pressed = false;
        }
        if (keystroke == -94 && function_key) {
            input_to_text_altgr_pressed = false;
        }
        return;
    }
    if (keystroke > 105) {
        return;
    }
    if (function_key) {
        keycount++;
        if (keycount == 3) {
            function_key = false;
            keycount = 0;
        }
    }
    if (keystroke == 105) {
        function_key = true;
        return;
    }
    char* keystroke_str[3] = {keyboard_stringmap[keystroke][0],
                              keyboard_stringmap[keystroke][1],
                              keyboard_stringmap[keystroke][2]};
    if (input_to_text_keypress_callback) {
        input_to_text_on_keypress_callback(keystroke_str[0]);
    }
    if (keystroke_str[0] == NULL) {
        return;
    }
    if (strcmp("", keystroke_str[0]) == 0 && strcmp("", keystroke_str[1]) == 0 &&
        strcmp("", keystroke_str[2]) == 0) {
        return;
    }
    if (strcmp("ARROW_LEFT", keystroke_str[0]) == 0) {
        if (cursor > 0) {
            cursor -= 1;
        }
        return;
    }
    if (strcmp("ARROW_RIGHT", keystroke_str[0]) == 0) {
        if (cursor < strlen(input_to_text_field)) {
            cursor += 1;
        }
        return;
    }
    if (strcmp("ARROW_UP", keystroke_str[0]) == 0) {
        if (input_to_text_arrow_up_callback) {
            input_to_text_on_arrow_up_callback(input_to_text_field);
        }
    }
    if (strcmp("ARROW_DOWN", keystroke_str[0]) == 0) {
        if (input_to_text_arrow_down_callback) {
            input_to_text_on_arrow_down_callback(input_to_text_field);
        }
    }
    if (strcmp("SHIFT", keystroke_str[0]) == 0 ||
        strcmp("SHIFT_R", keystroke_str[0]) == 0) {
        input_to_text_shift_pressed = true;
        return;
    }
    if (strcmp("ALT", keystroke_str[0]) == 0 && function_key == true /*altgr*/) {
        input_to_text_altgr_pressed = true;
        return;
    }
    if (strcmp("CAPS", keystroke_str[0]) == 0) {
        input_to_text_caps_pressed = !input_to_text_caps_pressed;
        return;
    }
    if (strcmp("ENTER", keystroke_str[0]) == 0) {
        if (input_to_text_enter_callback) {
            input_to_text_on_enter_callback(input_to_text_field);
        }
        return;
    }
    if (strcmp("ALT", keystroke_str[0]) == 0) {
        return;
    }
    if (strcmp("CTRL", keystroke_str[0]) == 0) {
        return;
    }
    if (strcmp("DEL", keystroke_str[0]) == 0) {
        if (cursor + 1 > strlen(input_to_text_field)) {
            return;
        }
        char input_to_text_field_cpy[strlen(input_to_text_field)];
        strcpy(input_to_text_field_cpy, input_to_text_field);
        str_remove_inbetween(input_to_text_field, input_to_text_field_cpy, cursor);

        if (input_to_text_string_change_callback_callback) {
            input_to_text_on_string_change_callback(input_to_text_field);
        }
        return;
    }

    if (strcmp("BACKSPACE", keystroke_str[0]) == 0) {
        if (strlen(input_to_text_field) > 0) {
            char input_to_text_field_cpy[strlen(input_to_text_field)];
            strcpy(input_to_text_field_cpy, input_to_text_field);
            str_remove_inbetween(input_to_text_field, input_to_text_field_cpy,
                                 cursor - 1);
            cursor--;
            if (input_to_text_string_change_callback_callback) {
                input_to_text_on_string_change_callback(input_to_text_field);
            }
            return;
        }
    }
    if (strcmp("SPACE", keystroke_str[0]) == 0) {
        strcat_inbetween(input_to_text_field, input_to_text_field, " ", cursor);
        cursor++;
        if (input_to_text_string_change_callback_callback) {
            input_to_text_on_string_change_callback(input_to_text_field);
        }
        return;
    }
    if (strcmp("TAB", keystroke_str[0]) == 0) {
        strcat_inbetween(input_to_text_field, input_to_text_field, "\t", cursor);
        if (input_to_text_string_change_callback_callback) {
            input_to_text_on_string_change_callback(input_to_text_field);
        }
        cursor++;
        return;
    }
    if (input_to_text_altgr_pressed) {
        if (keystroke_str[2] == NULL) {
            strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[0],
                             cursor);
            return;
        }
        strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[2],
                         cursor);

        cursor++;
        if (input_to_text_string_change_callback_callback) {
            input_to_text_on_string_change_callback(input_to_text_field);
        }
        return;
    }
    if (input_to_text_shift_pressed) {
        if (input_to_text_caps_pressed) {
            strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[0],
                             cursor);

            cursor++;
            if (input_to_text_string_change_callback_callback) {
                input_to_text_on_string_change_callback(input_to_text_field);
            }
            return;
        }
        if (keystroke_str[1] == NULL) {
            strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[0],
                             cursor);
            return;
        }
        strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[1],
                         cursor);

        cursor++;
        if (input_to_text_string_change_callback_callback) {
            input_to_text_on_string_change_callback(input_to_text_field);
        }
        return;
    }
    if (input_to_text_caps_pressed) {
        if (keystroke_str[1] == NULL) {
            strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[0],
                             cursor);
            return;
        }
        strcat_inbetween(input_to_text_field, input_to_text_field, keystroke_str[1],
                         cursor);
        cursor++;
        if (input_to_text_string_change_callback_callback) {
            input_to_text_on_string_change_callback(input_to_text_field);
        }
        return;
    }
    char input_to_text_field_cpy[strlen(input_to_text_field)];
    strcpy(input_to_text_field_cpy, input_to_text_field);
    strcat_inbetween(input_to_text_field, input_to_text_field_cpy, keystroke_str[0],
                     cursor);

    cursor++;
    if (input_to_text_string_change_callback_callback) {
        input_to_text_on_string_change_callback(input_to_text_field);
    }
}

char* input_to_text_field_get() {
    return input_to_text_field;
}

void input_to_text_init() {
    read_input_init_listener(&input_to_text_listener);
    input_to_text_listener.keystroke_handler = &input_to_text_handle_keystroke;
}

void input_to_text_add_enter_callback(void (*callback)(char*)) {
    input_to_text_on_enter_callback = callback;
    input_to_text_enter_callback = true;
}

void input_to_text_add_string_change_callback_callback(void (*callback)(char*)) {
    input_to_text_on_string_change_callback = callback;
    input_to_text_string_change_callback_callback = true;
}
void input_to_text_add_keypress_callback(void (*callback)(char*)) {
    input_to_text_on_keypress_callback = callback;
    input_to_text_keypress_callback = true;
}
void input_to_text_add_arrow_up_callback(void (*callback)(char*)) {
    input_to_text_on_arrow_up_callback = callback;
    input_to_text_arrow_up_callback = true;
}
void input_to_text_add_arrow_down_callback(void (*callback)(char*)) {
    input_to_text_on_arrow_down_callback = callback;
    input_to_text_arrow_down_callback = true;
}

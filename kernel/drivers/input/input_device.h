#pragma once

#include <types.h>

/**
 * @name: name for identification and searching
 * @type: type of device (mouse, keyboard, joystick, ...)
 * @keymap: keymap for device, input bit to standartized keycode (e.g. keymap[0] is "a"
 * key so: keymap[0] = 0x1)])
 * @has_keymap: true if keymap is defined
 * @handler: pointer to function which will be called when key is pressed
 */
typedef struct {
    char* name;
    char* type;
    char keymap[512];
    bool has_keymap;
    void (*handler)(uint8_t);
} input_device_t;

typedef struct {
    uint8_t id[64];
    char* name[64];
    char* type[64];
    char keymap[64][512];
    bool has_keymap[64];
} input_device_info_t;

uint8_t input_device_create_device(char* name, char* type, char keymap[512],
                                   void (*handler)(uint8_t));
void input_device_set_keymap(uint8_t device_id, char keymap[512]);
void input_device_send_key(uint8_t device_id, uint8_t key);
input_device_info_t input_device_get_info();
input_device_info_t input_device_of_type_get_info(char* type);

#pragma once

#include <types.h>

/**
 * @name: name of the input_reader
 * @callback: callback function to be called when a key is pressed
 * @used: defines if listener is used or can be reassigned
 */
typedef struct input_device_listener {
    char* name;
    void (*callback)(uint8_t);
    bool used;
} input_device_listener;

/**
 * @name: name for identification and searching
 * @type: type of device (mouse, keyboard, joystick, ...)
 * @keymap: keymap for device, input bit to standartized keycode (e.g. keymap[0] is "a"
 * key so: keymap[0] = 0x1)])
 * @listeners: array of function pointers that will be called when key is pressed
 */
typedef struct input_device {
    char* name;
    char* type;

    char keymap[512];

    struct input_device_listener listeners[64];
} input_device;

typedef struct input_device_info {
    uint8_t id[64];
    char* name[64];
    char* type[64];
    char keymap[64][512];
} input_device_info;

uint8_t input_device_create_device(char* name, char* type, char keymap[512]);
void input_device_set_keymap(uint8_t device_id, char keymap[512]);
uint8_t input_device_add_listener(uint8_t device_id, char* name,
                                  void (*callback)(uint8_t));
void input_device_remove_listener(uint8_t device_id, uint8_t listener_id);
void input_device_send_key(uint8_t device_id, uint8_t key);
input_device_info input_device_get_info();
input_device_info input_device_of_type_get_info(char* type);

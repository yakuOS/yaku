#include "input_device.h"
#include <string.h>
#include <types.h>
input_device devices[64];
uint8_t device_count = 0;

/**
 * @return uint8_t device "id"
 */
uint8_t input_device_create_device(char* name, char* type, char keymap[512]) {
    devices[device_count].name = name;
    devices[device_count].type = type;

    for (int i = 0; i < 512; i++) {
        devices[device_count].keymap[i] = keymap[i];
    }

    device_count++;
    return device_count - 1;
}
void input_device_set_keymap(uint8_t device_id, char keymap[512]) {
    for (int i = 0; i < 512; i++) {
        devices[device_id].keymap[i] = keymap[i];
    }
}
input_device_info input_device_get_info() {
    input_device_info info;

    for (int i = 0; i < device_count; i++) {
        info.id[i] = i;
        info.name[i] = devices[i].name;
        info.type[i] = devices[i].type;
        for (int j = 0; j < 512; j++) {
            info.keymap[i][j] = devices[i].keymap[j];
        }
    }
    return info;
}

input_device_info input_device_of_type_get_info(char* type) {
    input_device_info info;

    for (int i = 0; i < device_count; i++) {
        if (strcmp(devices[i].type, type) == 0) {
            info.id[i] = i;
            info.name[i] = devices[i].name;
            info.type[i] = devices[i].type;
            for (int j = 0; j < 512; j++) {
                info.keymap[i][j] = devices[i].keymap[j];
            }
        }
    }
    return info;
}

/**
 * @return id used to remove input_device_listener
 */
uint8_t input_device_add_listener(uint8_t device_id, char* name,
                                  void (*callback)(uint8_t)) {
    for (uint8_t i = 0; i < 64; i++) {
        if (devices[device_id].listeners[i].used != true) {
            devices[device_id].listeners[i].name = name;
            devices[device_id].listeners[i].callback = callback;
            devices[device_id].listeners[i].used = true;
            return i;
        }
    }

    return 0;
}

void input_device_remove_listener(uint8_t device_id, uint8_t listener_id) {
    devices[device_id].listeners[listener_id].name = NULL;
    devices[device_id].listeners[listener_id].callback = NULL;
    devices[device_id].listeners[listener_id].used = false;
}

void input_device_send_key(uint8_t device_id, uint8_t key) {
    for (uint8_t i = 0; i < 64; i++) {
        if (devices[device_id].listeners[i].used == true) {
            devices[device_id].listeners[i].callback(devices[device_id].keymap[key - 1]);
        }
    }
}

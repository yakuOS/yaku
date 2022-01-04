#include "string.h"
#include <drivers/serial.h>
#include <string.h>
#include <types.h>

/**
 * dont use same pointer for buffer and another string, copy it before using
 * @buffer: buffer to store the new string in. (minimal length: len(str1)+len(str2)+1)
 */
void strcat_inbetween(char* buffer, const char* str1, const char* str2, size_t index) {
    if (index >= strlen(str1)) {
        strcpy(buffer, str1);
        buffer[strlen(str1)] = '\0';
        strcat(buffer, str2);
        return;
    }
    if (index < 0) {
        index = 0;
    }
    strncpy(buffer, str1, index);
    buffer[index] = '\0';
    strcat(buffer, str2);
    strcat(buffer, str1 + index);
}

void str_remove_inbetween(char* buffer, const char* str1, size_t index) {
    uint16_t len = strlen(str1);
    if (len <= 0) {
        strcpy(buffer, str1);
        return;
    }
    if (index >= len) {
        index = len;
        strncpy(buffer, str1, index - 2);
        buffer[index - 1] = '\0';
        return;
    }
    if (index < 0) {
        index = 0;
    }
    strncpy(buffer, str1, index);
    buffer[index] = '\0';
    strcat(buffer, str1 + index + 1);
}

#include "string.h"
#include <types.h>

bool strcmp(char* str1, char* str2) {
    while (*str1 == *str2) {
        if (*str1 == '\0') {
            return true;
        }
        str1++;
        str2++;
    }
    return false;
}

void memcpy(void* dest, const void* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}
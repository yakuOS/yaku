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

size_t strnlen(const char* s, size_t maxlen) {
    const char* p = s;
    while (maxlen-- && *p) {
        p++;
    }
    return p - s;
}

size_t strlen(const char* s) {
    const char* p = s;
    while (*p) {
        p++;
    }
    return p - s;
}

char* strncat(char* s1, const char* s2, size_t n) {
    char* s = s1;
    /* Find the end of S1.  */
    s1 += strlen(s1);
    size_t ss = strnlen(s2, n);
    s1[ss] = '\0';
    memcpy(s1, s2, ss);
    return s;
}

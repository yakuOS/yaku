#include "string.h"
#include <types.h>

bool string_strcmp(char* str1, char* str2) {
    while (*str1 == *str2) {
        if (*str1 == '\0') {
            return true;
        }
        str1++;
        str2++;
    }
    return false;
}

void string_memcpy(void* dest, const void* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}

/**
 * @return size_t length of string, not counting further than maxlen
 */
size_t string_strnlen(const char* s, size_t maxlen) {
    const char* p = s;
    while (maxlen-- && *p) {
        p++;
    }
    return p - s;
}

/** 
 * @return size_t length of string
 */
size_t string_strlen(const char* s) {
    const char* p = s;
    while (*p) {
        p++;
    }
    return p - s;
}

char* string_strncat(char* s1, const char* s2, size_t n) {
    char* s = s1;
    /* Find the end of S1.  */
    s1 += string_strlen(s1);
    size_t ss = string_strnlen(s2, n);
    s1[ss] = '\0';
    string_memcpy(s1, s2, ss);
    return s;
}

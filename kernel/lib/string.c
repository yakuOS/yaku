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

/**
 * @return size_t length of string, not counting further than maxlen
 */
size_t strnlen(const char* s, size_t maxlen) {
    const char* p = s;
    while (maxlen-- && *p) {
        p++;
    }
    return p - s;
}

/**
 * @return size_t length of string
 */
size_t strlen(const char* s) {
    const char* p = s;
    while (*p) {
        p++;
    }
    return p - s;
}

void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}
void strncpy(char* dest, const char* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
    dest[n] = '\0';
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


char* strcat(char* s1, const char* s2) {
    char* s = s1;
    /* Find the end of S1.  */
    s1 += strlen(s1);
    size_t ss = strlen(s2);
    s1[ss] = '\0';
    memcpy(s1, s2, ss);
    return s;
}

char* strcat_inbetween(const char* s1, const char* s2, size_t index){
    if (index > strlen(s1)) {
        return strcat(s1, s2);
    }
    char* str3;
    strncpy(str3, s1, index);
    str3[index] = '\0';
    strcat(str3, s2);
    strcat(str3, s1 + index);
    return str3;
}

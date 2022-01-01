#pragma once

#include <types.h>

bool strcmp(char* a, char* b);
void memcpy(void* dest, const void* src, size_t n);
char* strncat(char* s1, const char* s2, size_t n);
size_t strnlen(const char* str, size_t maxlen);
size_t strlen(const char* s);
char* strcat(char* s1, const char* s2);
char* strcat_inbetween(const char* s1, const char* s2, size_t index);
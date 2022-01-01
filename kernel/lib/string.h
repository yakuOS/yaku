#pragma once

#include <types.h>

#define memcpy string_memcpy

bool string_strcmp(char* a, char* b);
void string_memcpy(void* dest, const void* src, size_t n);
char* string_strncat(char *s1, const char *s2, size_t n);
size_t string_strnlen(const char* str, size_t maxlen);
size_t string_strlen(const char* s);

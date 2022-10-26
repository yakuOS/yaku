#pragma once
#include <types.h>

typedef unsigned char uuid_t[16];
void uuid_unparse_lower(uuid_t uuid_buffer, char* uuid_string);
void uuid_generate_random(uuid_t uuid_buffer);

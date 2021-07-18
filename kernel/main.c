#include "multiboot.h"
#include "types.h"

void write_string(const char* string, uint8_t style) {
    uint8_t* vmem = (uint8_t*)0xB8000;
    while (*string != 0) {
        *vmem++ = *string++;
        *vmem++ = style;
    }
}

void kmain(multiboot_info_t* mb_info) {
    write_string("Hello, there !", 15);
};

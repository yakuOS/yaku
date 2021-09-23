#include "isr.h"

#include <drivers/vga_text.h>
#include <interrupts/pic.h>
#include <io.h>
#include <printf.h>

void isr_exception_handler(isr_xframe_t* frame) {
    if (frame->base_frame.vector == 13) {
        uint8_t scan_code = io_inb(0x60);

        char buffer[15];
        snprintf(buffer, 15, "SCANCODE: %d\n", scan_code);
        vga_text_puts(buffer);

        pic_send_eoi(1);
    } else {
        char buffer[15];
        snprintf(buffer, 15, "EXCEPTION: %llu\n", frame->base_frame.vector);
        vga_text_puts(buffer);
        for (;;) {}
    }
}

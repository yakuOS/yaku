#include "isr.h"

#include <drivers/vga_text.h>
#include <interrupts/pic.h>
#include <io.h>
#include <printf.h>

void isr_exception_handler(isr_xframe_t* frame) {
    char buffer[20];
    snprintf(buffer, 20, "EXCEPTION: %llu %llu\n", frame->base_frame.vector,
             frame->base_frame.error_code);
    vga_text_puts(buffer);
    asm("cli; hlt");
}

void isr_irq0(isr_xframe_t* frame) {}

void isr_irq1(isr_xframe_t* frame) {
    uint8_t scan_code = io_inb(0x60);

    char buffer[15];
    snprintf(buffer, 15, "SCANCODE: %d\n", scan_code);
    vga_text_puts(buffer);
}

void isr_irq2(isr_xframe_t* frame) {}
void isr_irq3(isr_xframe_t* frame) {}
void isr_irq4(isr_xframe_t* frame) {}
void isr_irq5(isr_xframe_t* frame) {}
void isr_irq6(isr_xframe_t* frame) {}
void isr_irq7(isr_xframe_t* frame) {}
void isr_irq8(isr_xframe_t* frame) {}
void isr_irq9(isr_xframe_t* frame) {}
void isr_irq10(isr_xframe_t* frame) {}
void isr_irq11(isr_xframe_t* frame) {}
void isr_irq12(isr_xframe_t* frame) {}
void isr_irq13(isr_xframe_t* frame) {}
void isr_irq14(isr_xframe_t* frame) {}
void isr_irq15(isr_xframe_t* frame) {}

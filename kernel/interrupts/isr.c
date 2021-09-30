#include "isr.h"

#include <drivers/vga_text.h>
#include <interrupts/pic.h>
#include <io.h>
#include <printf.h>

static const char* exception_messages[] = {
    "Divide-by-zero Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved error",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Security Exception",
    "Reserved error",
};

static const char* exception_message(uint64_t vector_number) {
    if (vector_number >= 21 && vector_number <= 29) {
        return "Reserved error";
    } else if (vector_number > 29) {
        return exception_messages[vector_number - 9];
    }

    return exception_messages[vector_number];
}

void isr_exception_handler(isr_xframe_t* frame) {

    const char* exception_msg = exception_message(frame->base_frame.vector);

    char buffer[50];
    snprintf(buffer, 50, "EXCEPTION: %s (%llu, %llu)\n", exception_msg,
             frame->base_frame.vector, frame->base_frame.error_code);
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

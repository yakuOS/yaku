#include "isr.h"
#include "pic.h"
#include <drivers/vga_text.h>
#include <io.h>
#include <printf.h>

void isr_exception_handler(isr_xframe_t* frame);
void isr_exception_handler(isr_xframe_t* frame) {

    switch (frame->base_frame.vector) {
    case 0:
        // vga_text_puts("ERROR - ISR: 0\n");

        break;
    case 1:
        vga_text_puts("ERROR - ISR: 1");
        break;
    case 2:
        vga_text_puts("ERROR - ISR: 2");
        break;
    case 3:
        vga_text_puts("ERROR - ISR: 3");
        break;
    case 4:
        vga_text_puts("ERROR - ISR: 4");
        break;
    case 5:
        vga_text_puts("ERROR - ISR: 5");
        break;
    case 6:
        vga_text_puts("ERROR - ISR: 6");
        break;
    case 7:
        vga_text_puts("ERROR - ISR: 7");
        break;
    case 8:
        vga_text_puts("ERROR - ISR: 8");
        break;
    case 9:
        vga_text_puts("ERROR - ISR: 9");
        break;
    case 10:
        vga_text_puts("ERROR - ISR: 10");
        break;
    case 11:
        vga_text_puts("ERROR - ISR: 11");
        break;
    case 12:
        vga_text_puts("ERROR - ISR: 12");
        break;
    case 13:
        //("ERROR - ISR: 13\n");

        // print byte sent by keyboard
        char buffer[20];
        sprintf(buffer, "%d", inb(0x60));
        vga_text_puts(buffer);

        // send eoi
        outb(0x20, 0x20);
        outb(0xa0, 0x20);

        __asm__ volatile("cli; hlt");
        break;
    case 14:
        vga_text_puts("ERROR - ISR: 14");
        break;
    case 15:
        vga_text_puts("ERROR - ISR: 15");
        break;
    case 16:
        vga_text_puts("ERROR - ISR: 16");
        break;
    case 17:
        vga_text_puts("ERROR - ISR: 17");
        break;
    case 18:
        vga_text_puts("ERROR - ISR: 18");
        break;
    case 19:
        vga_text_puts("ERROR - ISR: 19");
        break;
    case 20:
        vga_text_puts("ERROR - ISR: 20");
        break;
    case 21:
        vga_text_puts("ERROR - ISR: 21");
        break;
    case 22:
        vga_text_puts("ERROR - ISR: 22");
        break;
    case 23:
        vga_text_puts("ERROR - ISR: 23");
        break;
    case 24:
        vga_text_puts("ERROR - ISR: 24");
        break;
    case 25:
        vga_text_puts("ERROR - ISR: 25");
        break;
    case 26:
        vga_text_puts("ERROR - ISR: 26");
        break;
    case 27:
        vga_text_puts("ERROR - ISR: 27");
        break;
    case 28:
        vga_text_puts("ERROR - ISR: 28");
        break;
    case 29:
        vga_text_puts("ERROR - ISR: 29");
        break;
    case 30:
        vga_text_puts("ERROR - ISR: 30");
        break;
    case 31:
        vga_text_puts("ERROR - ISR: 31");
        break;
    }
}

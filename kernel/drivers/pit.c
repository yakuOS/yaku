#include <types.h>
#include <io.h>
#include <interrupts/idt.h>
#include <drivers/vga_text.h>
#include <printf.h>

uint32_t tick = 0;


void pit_increase(){
    tick++;
}

void pit_init(uint32_t frequency){
    uint32_t divisor = 1193180 / frequency;

    io_outb(0x43, 0x36);

    uint8_t lower = (uint8_t)(divisor & 0xFF);
    uint8_t upper = (uint8_t)( (divisor>>8) & 0xFF );

    io_outb(0x40, lower);
    io_outb(0x40, upper);
}

uint32_t pit_tick_get(){
    return tick;
}

uint32_t pit_read(){
    uint32_t count = 0;

    asm volatile("cli");
 
	io_outb(0x43,0b0000000);
 
	count = io_inb(0x40);
	count |= io_inb(0x40)<<8;

    asm volatile("sti");
 
	return count;
}

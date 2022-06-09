# Sources
Here are the sources we used to inform ourselves and which inspired our code (modified or copied). 
(It is possible that some sources are used in more places than indicated). \
One source that guided us through the entire process of programming an operating system was [osdev.org](https://www.osdev.org/)
```bash
│   kernel.c
│   link.ld
│   meson.build
│
├───memory
│       pmm
│         http://www.brokenthorn.com/Resources/OSDev17.html 
├───drivers
│   │   serial
│   │     https://wiki.osdev.org/Serial_Ports 
│   │   pit
│   │     https://wiki.osdev.org/Programmable_Interval_Timer 
│   │   rtc
│   │     https://wiki.osdev.org/RTC
│   │   vga_text
│   │     https://wiki.osdev.org/Text_UI 
│   ├───input
│   │       ps2:
│   │         https://wiki.osdev.org/%228042%22_PS/2_Controller
│   │         https://wiki.osdev.org/Mouse_Input
│   │         https://wiki.osdev.org/PS/2_Mouse
│   │         https://wiki.osdev.org/PS/2_Keyboard 
│   ├───lba
│         https://wiki.osdev.org/ATA_PIO_Mode
│         https://wiki.osdev.org/ATA_read/write_sectors
│         https://github.com/sirupsen/flying-cat/blob/master/os/drivers/hdd.asm 
│
├───lib
│   │
│   │
│   └───input
│   │         mouse_handler 
│   │           https://wiki.osdev.org/Mouse_Input
│   │           https://wiki.osdev.org/PS/2_Mouse
│   │   write_to_drive+file
│   │     https://www.cplusplus.com/reference/cstdio/
│   │   getopt
│   │     https://gist.github.com/attractivechaos/a574727fb687109a2adefcd75655d9ea 
│   │   atoi
│   │     https://www.geeksforgeeks.org/write-your-own-atoi/ 
│   │   stat
│   │     https://pubs.opengroup.org/onlinepubs/007904875/basedefs/sys/stat.h.html 
│
├───thirdparty
│   │
│   ├───printf
│   │       ...
│   │       https://github.com/mpaland/printf 
│   │
│   ├───string
│   │       ...
│   │       https://www.musl-libc.org/ 
│   │
│   └───math
│           ...
│           https://www.musl-libc.org/ 
│
├───interrupts
│       idt+isr
│          https://github.com/austanss/skylight/tree/trunk/glass/src/cpu/interrupts
│          https://wiki.osdev.org/Interrupt_Descriptor_Table
│          https://wiki.osdev.org/Interrupt_Service_Routines
│          https://wiki.osdev.org/Interrupts
│          https://wiki.osdev.org/I_Can%27t_Get_Interrupts_Working 
│          https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
│       pic
│          https://wiki.osdev.org/PIC 
├───resources
│       keyboard_keycode.h
│       keyboard_stringmap.h
│       keyboard_keymap.h
│       keyboard_keymap.c
│       keyboard_stringmap.c
│
├───multitasking
│     https://www.scs.stanford.edu/05au-cs240c/lab/amd64/AMD64-2.pdf#page=330 
│     http://6.s081.scripts.mit.edu/sp18/x86-64-architecture-guide.html
│     https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture
│
└───misc
│       stivale2
│         https://github.com/stivale/stivale 
│         https://github.com/stivale/stivale2-barebones 
│       io
│         https://wiki.osdev.org/Inline_Assembly/Examples#I.2FO_access
└───echfs
│     https://github.com/echfs/echfs 

```

# Sources we used
(It is possible that some sources are used in more places than mentioned)

│   kernel.c
│   link.ld
│   meson.build
│
├───memory
│       pmm
│         http://www.brokenthorn.com/Resources/OSDev17.html 
├───drivers
│   │   serial
│   │
│   │   pit
│   │
│   │   sse
│   │
│   │   pit
│   │
│   │   fpu
│   │
│   │   serial
│   │
│   │   vga_text
│   │
│   │   fpu
│   │
│   │   timer
│   │
│   │   rtc
│   │
│   │   vga_text
│   │
│   ├───input
│   │       ps2:
│   │         https://wiki.osdev.org/%228042%22_PS/2_Controller
│   │         https://wiki.osdev.org/Mouse_Input
│   │         https://wiki.osdev.org/PS/2_Mouse
│   │         https://wiki.osdev.org/PS/2_Keyboard 
│
├───lib
│   │
│   │   datetime
│   │
│   └───input
│           mouse_handler 
│             https://wiki.osdev.org/Mouse_Input
│             https://wiki.osdev.org/PS/2_Mouse
│
├───thirdparty
│   │
│   ├───printf
│   │       ...
│   │
│   ├───string
│   │       ...
│   │
│   └───math
│           ...
│
├───interrupts
│       isr.h
│       isr.c
│       pic.h
│       isr.asm
│       idt.c
│       pic.c
│       idt.asm
│       idt.h
│
├───resources
│       keyboard_keycode.h
│       keyboard_stringmap.h
│       keyboard_keymap.h
│       keyboard_keymap.c
│       keyboard_stringmap.c
│
├───multitasking
│       task.h
│       context_switch.asm
│       task.c
│       scheduler.c
│       scheduler.h
│
└───misc
│       stivale2
│         https://github.com/stivale/stivale 
│         https://github.com/stivale/stivale2-barebones 
│       io
│         https://wiki.osdev.org/Inline_Assembly/Examples#I.2FO_access 

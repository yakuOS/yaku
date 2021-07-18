ENTRY(start)
SECTIONS {
    . = 1M;
    .text : {
        /* ensure multiboot header comes first */
        *(.multiboot)
        *(.text)
    }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}

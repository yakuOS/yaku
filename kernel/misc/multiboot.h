#pragma once

#include <types.h>

#pragma pack(push, 1)

typedef struct {
    uint32_t tabsize, strsize, addr, _reserved;
} multiboot_symbol_table_t;

typedef struct {
    uint32_t num, size, addr, shndx;
} multiboot_section_header_table_t;

typedef struct {
    uint32_t flags, mem_lower, mem_upper, boot_device, cmdline, mods_count, mods_addr;

    union {
        multiboot_symbol_table_t symbol_table;
        multiboot_section_header_table_t section_header_table;
    };

    uint32_t mmap_length, mmap_addr, drives_length, drives_addr, config_table,
        boot_loader_name, apm_table, vbe_control_info, vbe_mode_info;

    uint16_t vbe_mode, vbe_interface_seg, vbe_interface_off, vbe_interface_len;

    uint64_t framebuffer_addr;

    uint32_t framebuffer_pitch, framebuffer_width, framebuffer_height, framebuffer_bpp,
        framebuffer_type;
} multiboot_info_t;

#pragma pack(pop)

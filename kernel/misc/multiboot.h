#pragma once

#include <types.h>

#pragma pack(push, 1)

#define MULTIBOOT_FB_TYPE_INDEXED 0
#define MULTIBOOT_FB_TYPE_RGB 1
#define MULTIBOOT_FB_TYPE_TEXT 2

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

    uint32_t framebuffer_pitch, framebuffer_width, framebuffer_height;

    uint8_t framebuffer_bpp, framebuffer_type;

    union {
        // indexed
        struct {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };
        // rgb
        struct {
            uint8_t framebuffer_red_field_position, framebuffer_red_mask_size,
                framebuffer_green_field_position, framebuffer_green_mask_size,
                framebuffer_blue_field_position, framebuffer_blue_mask_size;
        };
    };

} multiboot_info_t;

#pragma pack(pop)

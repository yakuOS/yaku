#include "pmm.h"
#include <drivers/serial.h>
#include <math.h>

void mm_init(stivale2_struct_tag_memmap_t* memory_map) {

    size_t memory_size_bytes = 0;
    for (uint32_t i = 0; i < memory_map->entries; i++) {
        serial_printf("Base: %p, Length: %i, Type: %i\n", memory_map->memmap[i].base,
                      memory_map->memmap[i].length, memory_map->memmap[i].type);
        if (memory_map->memmap[i].type == STIVALE2_MMAP_USABLE) {
            memory_size_bytes += memory_map->memmap[i].length;
        }
    }

    uint64_t* bitmap_location = 0;
    for (uint32_t i = 0; i < memory_map->entries; i++) {
        if (memory_map->memmap[i].type == STIVALE2_MMAP_USABLE) {
            bitmap_location = (uint64_t*)memory_map->memmap[i].base;
            break;
        }
    }

    pmm_init(memory_size_bytes / 1024, bitmap_location);

    for (uint32_t i = 0; i < memory_map->entries; i++) {

        uint64_t* address = (uint64_t*)memory_map->memmap[i].base;
        uint64_t length = memory_map->memmap[i].length;

        if (memory_map->memmap[i].type == STIVALE2_MMAP_USABLE) {
            pmm_init_region(address, length);
        } else {
            pmm_deinit_region(address, length);
        }
    }

    pmm_deinit_region(bitmap_location,
                      4096 * ceil(((double)pmm_get_block_count() / 8.0) / 4096.0));
}
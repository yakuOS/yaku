#include "pmm.h"

#include <drivers/serial.h>
#include <math.h>
#include <string.h>
#include <types.h>

static uint64_t pmm_memory_size = 0;
static uint64_t pmm_used_blocks = 0;
static uint64_t pmm_max_blocks = 0;
static uint64_t* pmm_memory_map = 0;

void pmm_mmap_set(uint64_t bit) {
    pmm_memory_map[bit / 32] |= (1 << (bit % 32));
}

void pmm_mmap_unset(uint64_t bit) {
    pmm_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

bool pmm_mmap_test(uint64_t bit) {
    return pmm_memory_map[bit / 32] & (1 << (bit % 32));
}

void pmm_init(stivale2_struct_tag_memmap_t* memory_map) {

    size_t memory_size_bytes = 0;
    for (uint32_t i = 0; i < memory_map->entries; i++) {
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

    pmm_memory_size = memory_size_bytes / 1024;
    pmm_memory_map = bitmap_location;
    pmm_max_blocks = (pmm_get_memory_size() * 1024) / PMM_BLOCK_SIZE;
    pmm_used_blocks = pmm_max_blocks;

    memset(pmm_memory_map, 0xF, pmm_get_block_count() / PMM_BLOCKS_PER_BYTE);

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

    serial_printf(
        "Initialized memory manager with %i KB of memory.\nTotal memory blocks: "
        "%i\nAvailable memory blocks: %lu\n\n",
        pmm_get_memory_size(), pmm_get_block_count(), pmm_get_free_block_count());
}

void pmm_init_region(uint64_t* base, size_t size) {

    uint64_t align = (uint64_t)base / PMM_BLOCK_SIZE;
    uint64_t blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--) {
        pmm_mmap_unset(align++);
        pmm_used_blocks--;
    }
}

void pmm_deinit_region(uint64_t* base, size_t size) {

    uint64_t align = (uint64_t)base / PMM_BLOCK_SIZE;
    uint64_t blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--) {
        pmm_mmap_set(align++);
        pmm_used_blocks++;
    }
}

void* pmm_alloc_block() {

    if (pmm_get_free_block_count() <= 0) {
        return 0;
    }

    uint64_t frame = pmm_mmap_find_first_free();

    if (frame == (uint64_t)-1) {
        return 0;
    }

    pmm_mmap_set(frame);

    uint64_t addr = frame * PMM_BLOCK_SIZE;
    pmm_used_blocks++;

    return (void*)addr;
}

void pmm_free_block(void* p) {
    uint64_t addr = (uint64_t)p;
    uint64_t frame = addr / PMM_BLOCK_SIZE;
    pmm_mmap_unset(frame);
    pmm_used_blocks--;
}

void* malloc(size_t size) {
    asm volatile("cli");
    if (size == 0) {
        return 0;
    }
    size = (size + 2 - 1) / PMM_BLOCK_SIZE + 1;
    if (pmm_get_free_block_count() <= size) {
        return 0;
    }

    uint64_t frame = pmm_mmap_find_first_free_size(size);

    if (frame == (uint64_t)-1) {
        return 0;
    }

    for (uint64_t i = 0; i < size; i++) {
        pmm_mmap_set(frame + i);
    }

    uint64_t addr = frame * PMM_BLOCK_SIZE;
    pmm_used_blocks += size;
    uint16_t blocks_allocated = (uint16_t)size;
    *((uint16_t*)addr) = blocks_allocated;
    asm volatile("sti");
    return ((void*)addr) + 2;
}

void free(void* p) {
    asm volatile("cli");
    if (p == 0) {
        return;
    }
    uint64_t addr = (uint64_t)p - 2;
    uint16_t size = *((uint16_t*)addr);
    uint64_t frame = addr / PMM_BLOCK_SIZE;


    for (uint64_t i = 0; i < size; i++) {
        pmm_mmap_unset(frame + i);
    }
    pmm_used_blocks -= size;
    asm volatile("sti");
}
void* calloc(size_t num, size_t size){
    uint64_t bytes_to_allocate = num*size;
    void* p = malloc(bytes_to_allocate);
    memset(p, 0, bytes_to_allocate);
    return p;
}
void* realloc(void* p, size_t new_size){
     if (p == 0) {
        return malloc(new_size);
    }
    uint64_t addr = (uint64_t)p-2;
    uint16_t size = *((uint16_t*)addr);
    uint64_t blocks_to_allocate = (new_size+2-1)/PMM_BLOCK_SIZE+1;
    if (blocks_to_allocate > size) {
        void* new_p = malloc(new_size);
        memcpy(new_p, p, size*PMM_BLOCK_SIZE-2);
        // serial_printf("Reallocating %i blocks to %i blocks\n", size, blocks_to_allocate);
        free(p);
        return new_p;
    } else {
        return p;
    }
} 

uint64_t pmm_mmap_find_first_free() {

    for (uint64_t i = 0; i < pmm_get_block_count() / 32; i++) {
        if (pmm_memory_map[i] != 0xFFFFFFFF) {
            for (uint64_t j = 0; j < 32; j++) {
                uint64_t bit = 1 << j;

                if (!(pmm_memory_map[i] & bit)) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }

    return -1;
}

uint64_t pmm_mmap_find_first_free_size(size_t size) {

    if (size == 0)
        return -1;

    if (size == 1)
        return pmm_mmap_find_first_free();

    for (uint64_t i = 0; i < pmm_get_block_count() / 32; i++) {
        if (pmm_memory_map[i] != 0xFFFFFFFF) {
            for (uint64_t j = 0; j < 32; j++) {

                uint64_t bit = 1 << j;

                if (!(pmm_memory_map[i] & bit)) {

                    uint64_t start_bit = i * 32;
                    start_bit += bit;

                    uint64_t free = 0;
                    for (uint64_t count = 0; count <= size; count++) {

                        if (!(pmm_mmap_test(start_bit + count))) {
                            free++;
                        }

                        if (free == size) {
                            return i * 4 * 8 + j;
                        }
                    }
                }
            }
        }
    }

    return -1;
}

size_t pmm_get_memory_size() {
    return pmm_memory_size;
}

uint64_t pmm_get_block_count() {
    return pmm_max_blocks;
}

uint64_t pmm_get_use_block_count() {
    return pmm_used_blocks;
}

uint64_t pmm_get_free_block_count() {
    return pmm_max_blocks - pmm_used_blocks;
}

uint64_t pmm_get_block_size() {
    return PMM_BLOCK_SIZE;
}

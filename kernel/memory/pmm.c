#include "pmm.h"
#include <drivers/serial.h>
#include <printf.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint64_t pmm_memory_size = 0;
static uint64_t pmm_used_blocks = 0;
static uint64_t pmm_max_blocks = 0;
static uint64_t* pmm_memory_map = 0;

void mmap_set(uint64_t bit) {
    pmm_memory_map[bit / 32] |= (1 << (bit % 32));
}

void mmap_unset(uint64_t bit) {
    pmm_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

bool mmap_test(uint64_t bit) {
    return pmm_memory_map[bit / 32] & (1 << (bit % 32));
}

void pmm_init(size_t memory_size, uint64_t* bitmap) {
    pmm_memory_size = memory_size;
    pmm_memory_map = bitmap;
    pmm_max_blocks = (pmm_get_memory_size() * 1024) / PMM_BLOCK_SIZE;
    pmm_used_blocks = pmm_max_blocks;

    memset(pmm_memory_map, 0xF, pmm_get_block_count() / PMM_BLOCKS_PER_BYTE);
}

void pmm_init_region(uint64_t* base, size_t size) {

    uint64_t align = (uint64_t)base / PMM_BLOCK_SIZE;
    uint64_t blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--) {
        mmap_unset(align++);
        pmm_used_blocks--;
    }
}

void pmm_deinit_region(uint64_t* base, size_t size) {

    uint64_t align = (uint64_t)base / PMM_BLOCK_SIZE;
    uint64_t blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--) {
        mmap_set(align++);
        pmm_used_blocks++;
    }
}

void* pmm_alloc_block() {

    if (pmm_get_free_block_count() <= 0) {
        return 0;
    }

    uint64_t frame = mmap_find_first_free();

    if (frame == (uint64_t)-1) {
        return 0;
    }

    mmap_set(frame);

    uint64_t addr = frame * PMM_BLOCK_SIZE;
    pmm_used_blocks++;

    return (void*)addr;
}

void pmm_free_block(void* p) {
    uint64_t addr = (uint64_t)p;
    uint64_t frame = addr / PMM_BLOCK_SIZE;
    mmap_unset(frame);
    pmm_used_blocks--;
}

void* malloc(size_t size) {

    if (pmm_get_free_block_count() <= size) {
        return 0;
    }

    uint64_t frame = mmap_find_first_free_size(size);

    if (frame == (uint64_t)-1) {
        return 0;
    }

    for (uint64_t i = 0; i < size; i++) {
        mmap_set(frame + i);
    }

    uint64_t addr = frame * PMM_BLOCK_SIZE;
    pmm_used_blocks += size;

    return (void*)addr;
}

void free(void* p, size_t size) {

    uint64_t addr = (uint64_t)p;
    uint64_t frame = addr / PMM_BLOCK_SIZE;

    for (uint64_t i = 0; i < size; i++) {
        mmap_unset(frame + i);
    }

    pmm_used_blocks -= size;
}

uint64_t mmap_find_first_free() {

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

uint64_t mmap_find_first_free_size(size_t size) {

    if (size == 0)
        return -1;

    if (size == 1)
        return mmap_find_first_free();

    for (uint64_t i = 0; i < pmm_get_block_count() / 32; i++) {
        if (pmm_memory_map[i] != 0xFFFFFFFF) {
            for (uint64_t j = 0; j < 32; j++) {

                uint64_t bit = 1 << j;

                if (!(pmm_memory_map[i] & bit)) {

                    uint64_t start_bit = i * 32;
                    start_bit += bit;

                    uint64_t free = 0;
                    for (uint64_t count = 0; count <= size; count++) {

                        if (!(mmap_test(start_bit + count))) {
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

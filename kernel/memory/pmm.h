#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stivale2.h>
#include <types.h>

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096

void mmap_set(uint64_t);
void mmap_unset(uint64_t);
bool mmap_test(uint64_t);
uint64_t mmap_find_first_free();
uint64_t mmap_find_first_free_size(size_t);

void pmm_init(size_t, uint64_t*);
void pmm_init_region(uint64_t*, size_t);
void pmm_deinit_region(uint64_t*, size_t);
void* pmm_alloc_block();
void pmm_free_block(void*);
void* malloc(size_t);
void free(void*, size_t);

size_t pmm_get_memory_size();
uint64_t pmm_get_block_count();
uint64_t pmm_get_use_block_count();
uint64_t pmm_get_free_block_count();
uint64_t pmm_get_block_size();

void mm_init(stivale2_struct_tag_memmap_t*);
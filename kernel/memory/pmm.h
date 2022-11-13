#pragma once

#include <stivale2.h>
#include <types.h>

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096

void pmm_mmap_set(uint64_t bit);
void pmm_mmap_unset(uint64_t bit);
bool pmm_mmap_test(uint64_t bit);
uint64_t pmm_mmap_find_first_free(void);
uint64_t pmm_mmap_find_first_free_size(size_t size);

void pmm_init(stivale2_struct_tag_memmap_t*);
void pmm_init_region(uint64_t* base, size_t size);
void pmm_deinit_region(uint64_t* base, size_t size);
void* pmm_alloc_block(void);
void pmm_free_block(void* p);
void* malloc(size_t size);
void free(void* p);

size_t pmm_get_memory_size(void);
uint64_t pmm_get_block_count(void);
uint64_t pmm_get_use_block_count(void);
uint64_t pmm_get_free_block_count(void);
uint64_t pmm_get_block_size(void);

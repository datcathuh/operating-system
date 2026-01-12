#pragma once

#include "types.h"

void mem_set(void *ptr, unsigned char value, size_t size);
void mem_copy(void *dst, const void *src, size_t size);

void *mem_alloc(size_t size);
void *mem_alloc_zero(size_t size);
void mem_free(void *ptr);
size_t mem_alloc_usable_size(void *ptr);

#define MEM_PAGE_PRESENT (1ULL << 0)
#define MEM_PAGE_WRITABLE (1ULL << 1)
#define MEM_PAGE_USER (1ULL << 2)
#define MEM_PAGE_PWT (1ULL << 3) // write-through
#define MEM_PAGE_PCD (1ULL << 4) // cache-disable
#define MEM_PAGE_PAT (1ULL << 7) // PAT bit in PTE
#define MEM_PAGE_PS (1ULL << 7)  // PS bit in PDE/PDPTE (do NOT use in PT)
#define MEM_PAGE_GLOBAL (1ULL << 8)
#define MEM_PAGE_WRITE_COMBINING (1ULL << 9) // purely OS-internal flag
#define MEM_PAGE_NX (1ULL << 63)
#define MEM_PAGE_NO_CACHE ((1ULL << 3) | (1ULL << 4)) // PWT | PCD
#define MEM_PAGE_MMIO                                                          \
	(MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE | MEM_PAGE_PWT | MEM_PAGE_PCD)
#define MEM_PAGE_ADDR_MASK 0x000FFFFFFFFFF000ULL
#define MEM_UPPER_FLAGS (MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE)

void mem_page_init(void);
void mem_page_map(uint64_t virt, uint64_t phys, uint64_t flags);
void mem_page_map_n(uint64_t virt, uint64_t phys, uint64_t count,
                    uint64_t flags);
void *mem_page_alloc(void);
void mem_page_free(void *addr, size_t npages);

void mem_page_debug_dump(void);
void mem_page_dump(uint64_t virt);

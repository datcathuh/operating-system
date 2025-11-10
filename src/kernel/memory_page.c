#include "memory.h"

extern uint64_t kernel_end;
static uint64_t next_free_page = 0;

void mem_page_init(void) {
    // Align up to 4 KiB
    next_free_page = ((uint64_t)&kernel_end + 0xFFF) & ~0xFFFULL;
}

void *mem_page_alloc(size_t npages) {
    uint64_t phys = next_free_page;
    next_free_page += 0x1000;

    // Identity-mapped: phys == virt
    void* ptr = (void*)phys;

    // MUST zero page tables
    mem_set(ptr, 0, 0x1000);

    return ptr;
}

void mem_page_free(void *addr, size_t npages) {}

static inline uint64_t* mem_pml4_get(void) {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    return (uint64_t*)(cr3 & ~0xFFFULL);
}

void mem_page_map(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t* pml4 = mem_pml4_get();

    int pml4_i = (virt >> 39) & 0x1FF;
    int pdpt_i = (virt >> 30) & 0x1FF;
    int pd_i   = (virt >> 21) & 0x1FF;
    int pt_i   = (virt >> 12) & 0x1FF;

    // PML4
    if (!(pml4[pml4_i] & MEM_PAGE_PRESENT)) {
        uint64_t* pdpt = mem_page_alloc(1);
        pml4[pml4_i] = (uint64_t)pdpt | MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE;
    }

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_i] & ~0xFFFULL);

    // PDPT
    if (!(pdpt[pdpt_i] & MEM_PAGE_PRESENT)) {
        uint64_t* pd = mem_page_alloc(1);
        pdpt[pdpt_i] = (uint64_t)pd | MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE;
    }

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_i] & ~0xFFFULL);

    // PD
    if (!(pd[pd_i] & MEM_PAGE_PRESENT)) {
        uint64_t* pt = mem_page_alloc(1);
        pd[pd_i] = (uint64_t)pt | MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE;
    }

    uint64_t* pt = (uint64_t*)(pd[pd_i] & ~0xFFFULL);

    // PT
    pt[pt_i] = (phys & ~0xFFFULL) | flags;

    // Flush TLB for this page
    __asm__ volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}

void mem_page_map_n(uint64_t virt, uint64_t phys,
					uint64_t count, uint64_t flags)
{
    for (uint64_t i = 0; i < count; i++) {
        mem_page_map(virt + i * 0x1000,
					 phys + i * 0x1000,
					 flags);
    }
}

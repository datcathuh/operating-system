#include "memory.h"
#include "serial.h"

extern uint64_t kernel_end;
static uint64_t next_free_page = 0;
static uint64_t pages_allocated = 0;

void mem_page_init(void) {
	// Align up to 4 KiB
	next_free_page = ((uint64_t)&kernel_end + 0xFFF) & ~0xFFFULL;
}

void *mem_page_alloc(void) {
	uint64_t phys = next_free_page;
	next_free_page += 0x1000;
	void *virt = (void *)phys;
	mem_set(virt, 0, 0x1000);
	pages_allocated++;
	return virt;
}

void mem_page_free(void *addr, size_t npages) {}

static inline uint64_t *mem_pml4_get(void) {
	uint64_t cr3;
	__asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
	return (uint64_t *)(cr3 & ~0xFFFULL);
}

void mem_page_map(uint64_t virt, uint64_t phys, uint64_t flags) {
	uint64_t *pml4 = mem_pml4_get();

	int pml4_i = (virt >> 39) & 0x1FF;
	int pdpt_i = (virt >> 30) & 0x1FF;
	int pd_i = (virt >> 21) & 0x1FF;
	int pt_i = (virt >> 12) & 0x1FF;

	// PML4
	if (!(pml4[pml4_i] & MEM_PAGE_PRESENT)) {
		uint64_t *pdpt = mem_page_alloc();
		pml4[pml4_i] = ((uint64_t)pdpt & MEM_PAGE_ADDR_MASK)| MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE;
	}

	uint64_t *pdpt = (uint64_t *)(pml4[pml4_i] & ~0xFFFULL);

	// PDPT
	if (!(pdpt[pdpt_i] & MEM_PAGE_PRESENT)) {
		uint64_t *pd = mem_page_alloc();
		pdpt[pdpt_i] = ((uint64_t)pd & MEM_PAGE_ADDR_MASK) | MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE;
	}

	uint64_t *pd = (uint64_t *)(pdpt[pdpt_i] & ~0xFFFULL);

	// PD
	if (!(pd[pd_i] & MEM_PAGE_PRESENT)) {
		uint64_t *pt = mem_page_alloc();
		pd[pd_i] = ((uint64_t)pt & MEM_PAGE_ADDR_MASK) | MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE;
	}

	uint64_t *pt = (uint64_t *)(pd[pd_i] & ~0xFFFULL);

	// PT
	pt[pt_i] = (phys & ~0xFFFULL) | flags;

	// Flush TLB for this page
	__asm__ volatile("invlpg (%0)" ::"r"(virt) : "memory");
}

void mem_page_map_n(uint64_t virt, uint64_t phys, uint64_t count,
                    uint64_t flags) {
	for (uint64_t i = 0; i < count; i++) {
		mem_page_map(virt + i * 0x1000, phys + i * 0x1000, flags);
	}
}

static inline void mem_page_debug_dump_4k(uint64_t va, uint64_t pa, uint64_t flags) {
    serial_puts("  MAP 4K  VA ");
    serial_put_hex64(va);
    serial_puts(" -> PA ");
    serial_put_hex64(pa);
    serial_puts(" flags= ");
    serial_put_hex32(flags);
    serial_puts("\n");
}

static void mem_page_debug_dump_4k_flush(uint64_t len,
										 uint64_t start_va, uint64_t start_pa,
										 uint64_t last_va,  uint64_t last_pa,
										 uint64_t flags) {
    if (len == 1) {
        mem_page_debug_dump_4k(start_va, start_pa, flags);
        return;
    }

    mem_page_debug_dump_4k(start_va, start_pa, flags);

	if(len - 2 > 0) {
		serial_puts("  ... ");
		serial_put_dec(len - 2);
		serial_puts(" adjacent pages with the same flags (");
		serial_put_dec((len - 2) * 0x1000);
		serial_puts(" bytes) ...\n");
	}

    mem_page_debug_dump_4k(last_va, last_pa, flags);
}

void mem_page_debug_dump(void) {
    serial_puts("Memory pagemap:\n");
	serial_puts("  Pages allocated: ");
	serial_put_dec(pages_allocated);
	serial_puts("\n");

	uint64_t *pml4 = mem_pml4_get();
    for (int pml4_i = 0; pml4_i < 512; pml4_i++) {
        uint64_t pml4e = pml4[pml4_i];
        if (!(pml4e & MEM_PAGE_PRESENT))
            continue;

        uint64_t *pdpt = (uint64_t *)(pml4e & MEM_PAGE_ADDR_MASK);

        for (int pdpt_i = 0; pdpt_i < 512; pdpt_i++) {
            uint64_t pdpte = pdpt[pdpt_i];
            if (!(pdpte & MEM_PAGE_PRESENT))
                continue;

            uint64_t virt_base_1g =
                ((uint64_t)pml4_i << 39) |
                ((uint64_t)pdpt_i << 30);

            if (pdpte & MEM_PAGE_PS) {
				serial_puts("  MAP 1G  VA ");
				serial_put_hex64(virt_base_1g);
				serial_puts(" -> PA ");
				serial_put_hex64(pdpte & MEM_PAGE_ADDR_MASK);
				serial_puts(" flags= ");
				serial_put_hex32(pdpte & 0xFFF);
				serial_puts("\n");
                continue;
            }

            uint64_t *pd = (uint64_t *)(pdpte & MEM_PAGE_ADDR_MASK);

            for (int pd_i = 0; pd_i < 512; pd_i++) {
                uint64_t pde = pd[pd_i];
                if (!(pde & MEM_PAGE_PRESENT))
                    continue;

                uint64_t virt_base_2m =
                    virt_base_1g |
                    ((uint64_t)pd_i << 21);

                if (pde & MEM_PAGE_PS) {
					serial_puts("  MAP 2M  VA ");
					serial_put_hex64(virt_base_2m);
					serial_puts(" -> PA ");
					serial_put_hex64(pde & MEM_PAGE_ADDR_MASK);
					serial_puts(" flags= ");
					serial_put_hex32(pde & 0xFFF);
					serial_puts("\n");
                    continue;
                }

                uint64_t *pt = (uint64_t *)(pde & MEM_PAGE_ADDR_MASK);

				uint64_t run_start_va = 0, run_start_pa = 0;
				uint64_t run_last_va  = 0, run_last_pa  = 0;
				uint64_t run_flags    = 0;
				uint64_t run_len      = 0;
				bool     in_run       = false;

				for (int pt_i = 0; pt_i < 512; pt_i++) {
					uint64_t pte = pt[pt_i];
					if (!(pte & MEM_PAGE_PRESENT))
						continue;

					uint64_t va = virt_base_2m | ((uint64_t)pt_i << 12);
					uint64_t pa = pte & MEM_PAGE_ADDR_MASK;
					uint64_t flags = pte & 0xFFF;

					if (!in_run) {
						// start new run
						in_run = true;
						run_start_va = run_last_va = va;
						run_start_pa = run_last_pa = pa;
						run_flags = flags;
						run_len = 1;
						continue;
					}

					bool contiguous =
						(va == run_last_va + 0x1000) &&
						(pa == run_last_pa + 0x1000) &&
						(flags == run_flags);

					if (contiguous) {
						run_last_va = va;
						run_last_pa = pa;
						run_len++;
					} else {
						mem_page_debug_dump_4k_flush(run_len,
													 run_start_va, run_start_pa,
													 run_last_va,  run_last_pa,
													 run_flags);

						// start new run
						run_start_va = run_last_va = va;
						run_start_pa = run_last_pa = pa;
						run_flags = flags;
						run_len = 1;
					}
				}

				if (in_run) {
					mem_page_debug_dump_4k_flush(run_len,
												 run_start_va, run_start_pa,
												 run_last_va,  run_last_pa,
												 run_flags);
				}
            }
        }
    }
}

void mem_page_dump(uint64_t virt) {
    serial_puts("Page information for: ");
	serial_put_hex64(virt);
	serial_puts("\n");

	uint64_t *pml4 = mem_pml4_get();

    int pml4_i = (virt >> 39) & 0x1FF;
    int pdpt_i = (virt >> 30) & 0x1FF;
    int pd_i   = (virt >> 21) & 0x1FF;
    int pt_i   = (virt >> 12) & 0x1FF;

    uint64_t pml4e = pml4[pml4_i];
    serial_puts("  PML4E = ");
    serial_put_hex64(pml4e);
    serial_puts("\n");

    if (!(pml4e & (1ULL << 0))) return;  // not present

    uint64_t *pdpt = (uint64_t *)(pml4e & 0x000FFFFFFFFFF000ULL);
    uint64_t pdpte = pdpt[pdpt_i];
    serial_puts("  PDPTE = ");
    serial_put_hex64(pdpte);
    serial_puts("\n");

    if (!(pdpte & (1ULL << 0))) return;  // not present

    if (pdpte & (1ULL << 7)) {           // 1GB page
        serial_puts("  Large 1G page, no PDE/PTE\n");
        return;
    }

    uint64_t *pd = (uint64_t *)(pdpte & 0x000FFFFFFFFFF000ULL);
    uint64_t pde = pd[pd_i];
    serial_puts("  PDE   = ");
    serial_put_hex64(pde);
    serial_puts("\n");

    if (!(pde & (1ULL << 0))) return;    // not present

    if (pde & (1ULL << 7)) {             // 2MB page
        serial_puts("  Large 2M page, no PTE\n");
        return;
    }

    uint64_t *pt = (uint64_t *)(pde & 0x000FFFFFFFFFF000ULL);
    uint64_t pte = pt[pt_i];
    serial_puts("  PTE   = ");
    serial_put_hex64(pte);
    serial_puts("\n");
}

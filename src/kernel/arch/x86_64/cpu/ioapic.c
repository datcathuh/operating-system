#include "arch/x86_64/entry/acpi.h"
#include "ioapic.h"
#include "memory.h"

#define IOAPIC_REGSEL 0x00
#define IOAPIC_WINDOW 0x10
#define IOAPIC_REDTBL 0x10

static inline void ioapic_write(uintptr_t ioapic_base, uint32_t reg,
                                uint32_t val) {
	*(volatile uint32_t *)(ioapic_base + IOAPIC_REGSEL) = reg;
	*(volatile uint32_t *)(ioapic_base + IOAPIC_WINDOW) = val;
}

static inline uint32_t ioapic_read(uintptr_t ioapic_base, uint32_t reg) {
	*(volatile uint32_t *)(ioapic_base + IOAPIC_REGSEL) = reg;
	return *(volatile uint32_t *)(ioapic_base + IOAPIC_WINDOW);
}

void ioapic_init(void) {
	struct acpi_ioapic *ioapics;
	int ioapic_count = 0;
	acpi_ioapic_get(&ioapics, &ioapic_count);

	if (!ioapic_count) {
		/* TODO: Kernel panic */
		return;
	}

	mem_page_map(ioapics[0].address, ioapics[0].address,
	             MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE | MEM_PAGE_NO_CACHE |
	                 MEM_PAGE_NX);
}

void ioapic_set_irq(uint32_t gsi, uint8_t vector, uint8_t lapic_id,
                    uint64_t flags) {
	struct acpi_ioapic *ioapics;
	int ioapic_count = 0;
	acpi_ioapic_get(&ioapics, &ioapic_count);

	if (!ioapic_count) {
		/* TODO: Kernel panic */
		return;
	}

	uint32_t pin = gsi - ioapics[0].gsi_base;

	uint64_t entry = 0;
	entry |= vector; // bits 0–7
	entry |= flags;
	entry |= ((uint64_t)lapic_id) << 56;

	ioapic_write(ioapics[0].address, IOAPIC_REDTBL + pin * 2, (uint32_t)entry);
	ioapic_write(ioapics[0].address, IOAPIC_REDTBL + pin * 2 + 1,
	             (uint32_t)(entry >> 32));
}

#include "lapic.h"
#include "memory.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_ENABLE (1ULL << 11)
#define IA32_APIC_BASE_X2APIC (1ULL << 10)

#define LAPIC_REG_ID 0x20
#define LAPIC_REG_EOI 0xB0
#define LAPIC_REG_SVR 0xF0
#define LAPIC_REG_LVT_TIMER 0x320
#define LAPIC_REG_LVT_ERROR 0x370
#define LAPIC_REG_TIMER_INITCNT 0x380
#define LAPIC_REG_TIMER_CURRCNT 0x390
#define LAPIC_REG_TIMER_DIV 0x3E0

static inline uint64_t rdmsr(uint32_t msr) {
	uint32_t lo, hi;
	__asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
	return ((uint64_t)hi << 32) | lo;
}

static uint64_t lapic_get_physical_base(void) {
	/* 0xFEE00000 is extremely common. But can be different.
	   We read this using a Model-Specific Register
	   (MSR) called IA32_APIC_BASE (0x1B). */

	uint64_t msr = rdmsr(IA32_APIC_BASE_MSR);

	if (!(msr & IA32_APIC_BASE_ENABLE)) {
		// This should never happen on modern systems
		return 0;
	}

	// Bits 12..35 contain the physical base address
	return (uint64_t)(msr & 0xFFFFF000ULL);
}

static inline void lapic_write(uint32_t reg, uint32_t val) {
	volatile uint32_t *lapic = (volatile uint32_t *)lapic_get_physical_base();
	lapic[reg / 4] = val;
	(void)lapic[reg / 4]; // Read-back to flush write buffer
}

void lapic_init(void) {
	uint64_t lapic_address = lapic_get_physical_base();
	volatile uint32_t *lapic = (volatile uint32_t *)lapic_address;

	mem_page_map(lapic_address, lapic_address,
	             MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE | MEM_PAGE_NO_CACHE);

	// Enable software APIC (bit 8 in SVR)
	uint32_t svr = lapic[LAPIC_REG_SVR / 4];
	svr |= 0x100; // APIC enable
	svr &= ~0xFF; // Set vector 0 for spurious IRQ (disable actual delivery)
	lapic_write(LAPIC_REG_SVR, svr);

	// Mask all LVT entries (disable timer, error, etc.)
	lapic_write(LAPIC_REG_LVT_TIMER, 1 << 16); // mask bit
	lapic_write(LAPIC_REG_LVT_ERROR, 1 << 16);

	// Send EOI just in case something was pending
	lapic_write(LAPIC_REG_EOI, 0);
}

uint8_t lapic_get_id(void) {
	volatile uint32_t *lapic = (volatile uint32_t *)lapic_get_physical_base();
	return (lapic[LAPIC_REG_ID / 4] >> 24) & 0xFF;
}

void lapic_eoi(void) {
	volatile uint32_t *lapic = (volatile uint32_t *)lapic_get_physical_base();
	lapic[LAPIC_REG_EOI / 4] = 0;
}

#include "idt.h"
#include "lapic.h"
#include "memory.h"
#include "io.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_ENABLE (1ULL << 11)
#define IA32_APIC_BASE_X2APIC (1ULL << 10)

// LAPIC register offsets (from base address)
#define LAPIC_ID 0x020            // LAPIC ID
#define LAPIC_VERSION 0x030       // LAPIC Version
#define LAPIC_TPR 0x080           // Task Priority Register
#define LAPIC_APR 0x090           // Arbitration Priority Register
#define LAPIC_PPR 0x0A0           // Processor Priority Register
#define LAPIC_EOI 0x0B0           // End of Interrupt
#define LAPIC_RRD 0x0C0           // Remote Read Register
#define LAPIC_LDR 0x0D0           // Logical Destination Register
#define LAPIC_DFR 0x0E0           // Destination Format Register
#define LAPIC_SVR 0x0F0           // Spurious Interrupt Vector Register
#define LAPIC_ISR0 0x100          // In-Service Register (bits 0-31)
#define LAPIC_ISR1 0x110          // In-Service Register (bits 32-63)
#define LAPIC_ISR2 0x120          // In-Service Register (bits 64-95)
#define LAPIC_ISR3 0x130          // In-Service Register (bits 96-127)
#define LAPIC_ISR4 0x140          // In-Service Register (bits 128-159)
#define LAPIC_ISR5 0x150          // In-Service Register (bits 160-191)
#define LAPIC_ISR6 0x160          // In-Service Register (bits 192-223)
#define LAPIC_ISR7 0x170          // In-Service Register (bits 224-255)
#define LAPIC_TMR0 0x180          // Trigger Mode Register (bits 0-31)
#define LAPIC_TMR1 0x190          // Trigger Mode Register (bits 32-63)
#define LAPIC_TMR2 0x1A0          // Trigger Mode Register (bits 64-95)
#define LAPIC_TMR3 0x1B0          // Trigger Mode Register (bits 96-127)
#define LAPIC_TMR4 0x1C0          // Trigger Mode Register (bits 128-159)
#define LAPIC_TMR5 0x1D0          // Trigger Mode Register (bits 160-191)
#define LAPIC_TMR6 0x1E0          // Trigger Mode Register (bits 192-223)
#define LAPIC_TMR7 0x1F0          // Trigger Mode Register (bits 224-255)
#define LAPIC_IRR0 0x200          // Interrupt Request Register (bits 0-31)
#define LAPIC_IRR1 0x210          // Interrupt Request Register (bits 32-63)
#define LAPIC_IRR2 0x220          // Interrupt Request Register (bits 64-95)
#define LAPIC_IRR3 0x230          // Interrupt Request Register (bits 96-127)
#define LAPIC_IRR4 0x240          // Interrupt Request Register (bits 128-159)
#define LAPIC_IRR5 0x250          // Interrupt Request Register (bits 160-191)
#define LAPIC_IRR6 0x260          // Interrupt Request Register (bits 192-223)
#define LAPIC_IRR7 0x270          // Interrupt Request Register (bits 224-255)
#define LAPIC_ESR 0x280           // Error Status Register
#define LAPIC_ICR_LOW 0x300       // Interrupt Command Register (bits 0-31)
#define LAPIC_ICR_HIGH 0x310      // Interrupt Command Register (bits 32-63)
#define LAPIC_TIMER_LVT 0x320     // Timer Local Vector Table Entry
#define LAPIC_THERMAL_LVT 0x330   // Thermal Local Vector Table Entry
#define LAPIC_PERF_LVT 0x340      // Performance Counter LVT Entry
#define LAPIC_LINT0_LVT 0x350     // Local Interrupt 0 LVT Entry
#define LAPIC_LINT1_LVT 0x360     // Local Interrupt 1 LVT Entry
#define LAPIC_ERROR_LVT 0x370     // Error LVT Entry
#define LAPIC_TIMER_INIT 0x380    // Timer Initial Count Register
#define LAPIC_TIMER_CURRENT 0x390 // Timer Current Count Register
#define LAPIC_TIMER_DIVIDE 0x3E0  // Timer Divide Configuration Register

// Timer modes (for LAPIC_TIMER_LVT)
#define LAPIC_TIMER_ONESHOT (0 << 17)
#define LAPIC_TIMER_PERIODIC (1 << 17)
#define LAPIC_TIMER_DEADLINE (2 << 17)

// Timer divide values (for LAPIC_TIMER_DIVIDE)
#define LAPIC_DIVIDE_BY_1 0x0B
#define LAPIC_DIVIDE_BY_2 0x00
#define LAPIC_DIVIDE_BY_4 0x01
#define LAPIC_DIVIDE_BY_8 0x02
#define LAPIC_DIVIDE_BY_16 0x03
#define LAPIC_DIVIDE_BY_32 0x08
#define LAPIC_DIVIDE_BY_64 0x09
#define LAPIC_DIVIDE_BY_128 0x0A

// Other useful flags
#define LAPIC_MASKED (1 << 16)
#define LAPIC_UNMASKED (0 << 16)
#define LAPIC_SVR_ENABLE (1 << 8)

void lapic_timer(void);

static volatile uint32_t *lapic_base = NULL;

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

static inline uint32_t lapic_read(uint32_t reg) {
	return lapic_base[reg >> 2]; // Divide by 4 since we're indexing uint32_t*
}

static inline void lapic_write(uint32_t reg, uint32_t val) {
	lapic_base[reg / 4] = val;
	(void)lapic_base[reg / 4]; // Read-back to flush write buffer
}

void lapic_init(void) {
	uint64_t lapic_address = lapic_get_physical_base();
	lapic_base = (volatile uint32_t *)lapic_address;

	mem_page_map(lapic_address, lapic_address,
	             MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE | MEM_PAGE_NO_CACHE);

	// Mask all LVT entries (disable timer, error, etc.)
	lapic_write(LAPIC_TIMER_LVT, 1 << 16); // mask bit
	lapic_write(LAPIC_ERROR_LVT, 1 << 16);

	// Send EOI just in case something was pending
	lapic_write(LAPIC_EOI, 0);

	// Enable LAPIC via Spurious Interrupt Vector Register
	lapic_write(LAPIC_SVR, lapic_read(LAPIC_SVR) | LAPIC_SVR_ENABLE | 0xFF);
}

uint8_t lapic_get_id(void) { return (lapic_base[LAPIC_ID / 4] >> 24) & 0xFF; }

void lapic_eoi(void) { lapic_base[LAPIC_EOI / 4] = 0; }

uint32_t lapic_calibrate_timer(void) {
	// Disable LAPIC timer during calibration
	lapic_write(LAPIC_TIMER_LVT, LAPIC_MASKED);
	lapic_write(LAPIC_TIMER_DIVIDE, 0x3); // divide by 16

	// Use PIT to measure 10ms
	io_outb(0x43, 0x30); // Channel 0, mode 0
	io_outb(0x40, 0xFF);
	io_outb(0x40, 0xFF); // Count = 65535 (~54ms at 1.193182 MHz)

	// Start LAPIC timer at maximum count
	lapic_write(LAPIC_TIMER_INIT, 0xFFFFFFFF);

	// Wait for PIT to complete (simple busy wait)
	uint8_t start = io_inb(0x40);
	while (io_inb(0x40) >= start)
		; // Wait one full cycle

	// Read how many LAPIC ticks elapsed
	uint32_t elapsed = 0xFFFFFFFF - lapic_read(LAPIC_TIMER_CURRENT);

	// Calculate ticks for desired frequency
	// This elapsed count is for ~54ms, scale to your desired rate
	return (elapsed / 54) * (1000 / 10); // For 10Hz (100ms intervals)
}

void lapic_timer_setup(uint8_t vector, uint32_t frequency_hz) {
	// 1. Set up IDT entry for timer interrupt
	idt_gate_set(vector, lapic_timer); // your asm handler

	// 2. Configure divide value (divide by 16)
	lapic_write(LAPIC_TIMER_DIVIDE, 0x3);

	// 3. Calibrate the timer (measure CPU bus frequency)
	// For now, using a typical value - you should calibrate this
	uint32_t bus_freq = 1000000000; // 1 GHz estimate
	uint32_t divisor = 16;
	uint32_t ticks_per_interrupt = (bus_freq / divisor) / frequency_hz;

	// 4. Set LVT Timer entry (vector + periodic mode + unmasked)
	lapic_write(LAPIC_TIMER_LVT, vector | LAPIC_TIMER_PERIODIC);

	// 5. Set initial count (starts the timer)
	lapic_write(LAPIC_TIMER_INIT, ticks_per_interrupt);
}

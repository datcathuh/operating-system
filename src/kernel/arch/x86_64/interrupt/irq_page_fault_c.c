#include "arch/x86_64/cpu/idt.h"
#include "cpu.h"
#include "irq_page_fault.h"
#include "memory.h"
#include "serial.h"

struct page_fault_frame {
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

static inline uint64_t page_fault_read_cr2(void) {
	uint64_t val;
	__asm__ volatile("mov %%cr2, %0" : "=r"(val));
	return val;
}

void irq_page_fault_asm(void);

void irq_page_fault_c(struct page_fault_frame *frame, uint64_t error) {
	uint64_t cr2 = page_fault_read_cr2();

	serial_puts("\nPANIC: PAGE FAULT\n");
	serial_puts("  address: ");
	serial_put_hex64(cr2);
	serial_puts("\n");

	serial_puts("  error: ");
	serial_put_hex64(error);
	serial_puts("\n  reason: ");

	if (!(error & 1))
		serial_puts("non-present ");
	else
		serial_puts("protection ");

	serial_puts((error & (1 << 1)) ? "write " : "read ");
	serial_puts((error & (1 << 2)) ? "user " : "supervisor ");

	if (error & (1 << 3))
		serial_puts("reserved-bit ");

	if (error & (1 << 4))
		serial_puts("instruction-fetch ");

	serial_puts("\n");

	serial_puts("  RIP: ");
	serial_put_hex64(frame->rip);
	serial_puts("\n");

	mem_page_dump(cr2);

	mem_page_debug_dump();

	for (;;) {
		cpu_halt();
	}
}

void irq_page_fault_register(void) { idt_gate_set(0x0E, irq_page_fault_asm); }

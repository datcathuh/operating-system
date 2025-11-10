#include "idt.h"
#include "irq_double_fault.h"
#include "serial.h"

static inline uint64_t page_fault_read_cr2(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(val));
    return val;
}

void irq_page_fault_asm(void);

void irq_page_fault_c(void) {
	uint64_t page = page_fault_read_cr2();

	serial_puts("PANIC: page_fault @ ");
	serial_put_hex64(page);
	serial_puts("\n");

	for (;;) {
		__asm__ volatile("hlt");
	}
}

void irq_page_fault_register(void) {
	idt_gate_set(0x8, irq_page_fault_asm);
}

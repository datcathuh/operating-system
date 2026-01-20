#include "arch/x86_64/cpu/idt.h"
#include "irq_gp.h"
#include "serial.h"

void irq_ud_asm(void);

void irq_ud_c(void) {
	serial_puts("PANIC: Undefined Opcode fault\n");
	for (;;) {
		__asm__ volatile("hlt");
	}
}

void irq_ud_register(void) { idt_gate_set(0x6, irq_ud_asm); }

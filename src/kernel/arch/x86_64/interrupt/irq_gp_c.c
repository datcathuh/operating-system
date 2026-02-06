#include "arch/x86_64/cpu/idt.h"
#include "cpu.h"
#include "irq_gp.h"
#include "serial.h"

void irq_gp_asm(void);

void irq_gp_c(void) {
	serial_puts("PANIC: General protection fault\n");
	for (;;) {
		cpu_halt();
	}
}

void irq_gp_register(void) { idt_gate_set(0x13, irq_gp_asm); }

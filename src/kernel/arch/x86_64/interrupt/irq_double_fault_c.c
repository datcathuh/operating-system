#include "arch/x86_64/cpu/idt.h"
#include "cpu.h"
#include "irq_double_fault.h"
#include "serial.h"

void irq_double_fault_asm(void);

void irq_double_fault_c(void) {
	serial_puts("PANIC: double_fault\n");
	for (;;) {
		cpu_halt();
	}
}

void irq_double_fault_register(void) {
	idt_gate_set(0x8, irq_double_fault_asm);
}

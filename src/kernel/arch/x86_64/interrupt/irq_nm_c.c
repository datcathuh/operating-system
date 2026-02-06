#include "arch/x86_64/cpu/idt.h"
#include "cpu.h"
#include "irq_nm.h"
#include "serial.h"

void irq_nm_asm(void);

void irq_nm_c(void) {
	serial_puts("PANIC: No Math / Device not found fault\n");
	for (;;) {
		cpu_halt();
	}
}

void irq_nm_register(void) { idt_gate_set(0x7, irq_nm_asm); }

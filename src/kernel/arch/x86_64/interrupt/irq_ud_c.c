#include "arch/x86_64/cpu/idt.h"
#include "cpu.h"
#include "irq_ud.h"
#include "serial.h"

void irq_ud_asm(void);

void irq_ud_c(void) {
	serial_puts("PANIC: Undefined Opcode fault\n");
	for (;;) {
		cpu_halt();
	}
}

void irq_ud_register(void) { idt_gate_set(0x6, irq_ud_asm); }

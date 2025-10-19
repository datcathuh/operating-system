#include "idt.h"

void irq_timer_asm(void);

void irq_timer_c(void) {
}

void irq_timer_register(void) {
	idt_gate_set(0x20, (uint32_t)irq_timer_asm);
}

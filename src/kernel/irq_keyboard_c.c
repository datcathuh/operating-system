#include "idt.h"

void irq_keyboard_asm(void);

void irq_keyboard_c(void) {
}

void irq_keyboard_register(void) {
	idt_gate_set(0x21, (uint32_t)irq_keyboard_asm);
}

#include "idt.h"
#include "io.h"
#include "lapic.h"

static uint32_t irq_key_count = 0;
#define irq_key_queue_length 32
static uint32_t irq_key_queue[irq_key_queue_length];

void irq_keyboard_asm(void);

void irq_keyboard_c(void) {
	uint8_t sc = io_inb(0x60);

	// TODO: Add locking
	if (irq_key_count >= irq_key_queue_length) {
		lapic_eoi();
		return;
	}

	irq_key_queue[irq_key_count] = sc;
	irq_key_count++;

	lapic_eoi();
}

uint32_t irq_keyboard_count(void) { return irq_key_count; }

bool irq_keyboard_consume_key(uint8_t *sc) {
	// TODO: Add locking
	if (irq_key_count == 0) {
		return false;
	}
	*sc = irq_key_queue[0];
	for (uint32_t i = 0; i < irq_key_count; i++) {
		irq_key_queue[i] = irq_key_queue[i + 1];
	}
	irq_key_count--;
	return true;
}

void irq_keyboard_register(void) { idt_gate_set(0x21, irq_keyboard_asm); }

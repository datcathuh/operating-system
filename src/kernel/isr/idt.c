#include "idt.h"
#include "types.h"

struct idt_entry {
	uint16_t offset_low;  // bits 0..15
	uint16_t selector;    // code segment selector
	uint8_t ist;          // bits 0..2 = IST index
	uint8_t type_attr;    // type, DPL, P
	uint16_t offset_mid;  // bits 16..31
	uint32_t offset_high; // bits 32..63
	uint32_t zero;
} __attribute__((packed));

struct idtr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct idt_entry idt[256] = {0};
struct idtr idtp;

void idt_gate_set(int n, void (*handler)()) {
	uint16_t selector = 0x08;
	uint8_t type_attr = 0x8E; // P=1, DPL=0, type=14
	uint8_t ist = 0;          // Use current stack

	uint64_t addr = (uint64_t)handler;
	idt[n].offset_low = addr & 0xFFFF;
	idt[n].selector = selector;
	idt[n].ist = ist & 0x7;
	idt[n].type_attr = type_attr;
	idt[n].offset_mid = (addr >> 16) & 0xFFFF;
	idt[n].offset_high = (addr >> 32) & 0xFFFFFFFF;
	idt[n].zero = 0;
}

static void idt_load(struct idtr *idt_ptr) {
	__asm__ volatile("lidt (%0)" : : "r"(idt_ptr));
}

void idt_init() {
	idtp.limit = sizeof(idt) - 1;
	idtp.base = (uint64_t)&idt;
	idt_load(&idtp);
}

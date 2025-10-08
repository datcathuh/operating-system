#include "idt.h"
#include <stdint.h>

struct IDTEntry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct IDTEntry idt[256];
struct IDTPtr idtp;

void idt_gate_set(int n, uint32_t handler) {
    idt[n].base_low = handler & 0xFFFF;
    idt[n].sel = 0x08; // code segment selector
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; // present, ring 0, 32-bit interrupt gate
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

static void idt_load(struct IDTPtr* idt_ptr) {
    __asm__ volatile("lidt (%0)" : : "r"(idt_ptr));
}

void idt_install() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;
    idt_load(&idtp);
}

#include "io.h"
#include "serial.h"

static inline void serial_outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void serial_putc(char c) {
	serial_outb(0x3F8, (uint8_t)c);
}

void serial_put_hex8(uint8_t v) {
    const char hex[] = "0123456789ABCDEF";
    serial_putc(hex[v >> 4]);
    serial_putc(hex[v & 0x0F]);
}

void serial_put_hex32(uint32_t val) {
    const char *hex = "0123456789ABCDEF";
    serial_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
		serial_putc(hex[(val >> i) & 0xF]);
	}
}

void serial_puts(const char *s) {
	while (*s) {
		serial_putc(*s++);
	}
}

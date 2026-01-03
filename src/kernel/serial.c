#include "io.h"
#include "serial.h"

void serial_putc(char c) {
	// while ((io_inb(0x3F8 + 5) & 0x20) == 0) {
	// }
	io_outb(0x3F8, (uint8_t)c);
}

void serial_put_dec(uint64_t v) {
	char buf[21]; // max 20 digits for uint64_t + '\0'
	int i = 0;

	if (v == 0) {
		serial_putc('0');
		return;
	}

	while (v > 0) {
		buf[i++] = '0' + (v % 10);
		v /= 10;
	}

	while (i > 0) {
		serial_putc(buf[--i]);
	}
}

void serial_put_hex8(uint8_t val) {
	const char hex[] = "0123456789ABCDEF";
	serial_puts("0x");
	serial_putc(hex[val >> 4]);
	serial_putc(hex[val & 0x0F]);
}

void serial_put_hex16(uint16_t val) {
	const char hex[] = "0123456789ABCDEF";
	serial_puts("0x");
	for (int i = 12; i >= 0; i -= 4) {
		serial_putc(hex[(val >> i) & 0xF]);
	}
}

void serial_put_hex32(uint32_t val) {
	const char *hex = "0123456789ABCDEF";
	serial_puts("0x");
	for (int i = 28; i >= 0; i -= 4) {
		serial_putc(hex[(val >> i) & 0xF]);
	}
}

void serial_put_hex64(uint64_t val) {
	const char *hex = "0123456789ABCDEF";
	serial_puts("0x");
	for (int i = 60; i >= 0; i -= 4) {
		serial_putc(hex[(val >> i) & 0xF]);
	}
}

void serial_puts(const char *s) {
	for (int i = 0; s[i] != 0; i++) {
		serial_putc(s[i]);
	}
}

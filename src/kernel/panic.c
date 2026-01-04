#include "panic.h"
#include "serial.h"

__attribute__((noreturn)) void panic(const char *msg) {
	__asm__ volatile("cli"); // Disable interrupts

	serial_puts("\n\n*** KERNEL PANIC ***\n");
	serial_puts(msg);
	serial_puts("\n\n");

	/* Halt forever */
	for (;;) {
		__asm__ volatile("hlt");
	}
}

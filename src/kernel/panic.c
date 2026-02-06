#include "cpu.h"
#include "interrupt.h"
#include "panic.h"
#include "serial.h"

__attribute__((noreturn)) void panic(const char *msg) {
	interrupt_stop();

	serial_puts("\n\n*** KERNEL PANIC ***\n");
	serial_puts(msg);
	serial_puts("\n\n");

	/* Halt forever */
	for (;;) {
		cpu_halt();
	}
}

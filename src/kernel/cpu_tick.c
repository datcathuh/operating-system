#include "cpu_tick.h"
#include "arch/x86_64/cpu/lapic.h"
#include "scheduler.h"

#include "serial.h"

uint64_t cpu_tick_count = 0;

void cpu_tick(struct cpu_context *interrupted_context) {
	lapic_eoi();

	cpu_tick_count++;

	/*
	serial_puts("tick: ");
	serial_put_dec(cpu_tick_count);
	serial_puts("\n");
	*/

	// schedule();

	schedule_from_interrupt(interrupted_context);
}

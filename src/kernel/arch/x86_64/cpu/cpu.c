#include "cpu.h"

void cpu_halt(void) {
	__asm__ volatile("hlt");
}

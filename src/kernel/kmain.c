#include "diagnostics.h"
#include "interrupt.h"
#include "kshell/kshell.h"
#include "kshell/kshell_bga.h"
#include "kshell/kshell_help.h"
#include "kshell/kshell_julia.h"
#include "kshell/kshell_mandelbrot.h"
#include "kshell/kshell_shutdown.h"
#include "kshell/kshell_snake.h"
#include "kshell/kshell_sysinfo.h"
#include "kshell/kshell_task.hpp"
#include "kshell/kshell_tetris.h"
#include "kshell/kshell_x.h"
#include "memory.h"
#include "pci.h"
#include "sleep.h"
#include "scheduler.h"
#include "task.h"

static struct task kmain_idle;
static struct task kmain_kshell;

void idle(void);

void kmain(uint64_t magic, void *mb_addr) {
	/* Init PCI by scanning the bus and find all devices there. This will
	   load drivers for everything it finds. */
	pci_init();

	pci_debug_dump();

	/* Allow interrupts again. */
	interrupt_resume();

	print_diagnostics();

	kshell_init();
	kshell_bga_register();
	kshell_help_register();
	kshell_julia_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();
	kshell_snake_register();
	kshell_sysinfo_register();
	kshell_task_register();
	kshell_tetris_register();
	kshell_x_register();

	mem_page_debug_dump();

	task_create("kidle", &kmain_idle, idle);
	scheduler_task_add(&kmain_idle);

	task_create("kshell", &kmain_kshell, kshell);
	scheduler_task_add(&kmain_kshell);

	scheduler_start();
}

void idle(void) {
	for (;;) {
		sleep_ms(10);
		yield();
	}
}

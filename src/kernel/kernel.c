#include "acpi.h"
#include "diagnostics.h"
#include "gdt.h"
#include "idt.h"
#include "isr/irq_double_fault.h"
#include "isr/irq_gp.h"
#include "isr/irq_page_fault.h"
#include "isr/irq_keyboard.h"
#include "isr/irq_timer.h"
#include "kshell/kshell.h"
#include "kshell/kshell_bga.h"
#include "kshell/kshell_help.h"
#include "kshell/kshell_julia.h"
#include "kshell/kshell_mandelbrot.h"
#include "kshell/kshell_shutdown.h"
#include "kshell/kshell_snake.h"
#include "kshell/kshell_tetris.h"
#include "lapic.h"
#include "memory.h"
#include "serial.h"
#include "pci.h"
#include "pic.h"
#include "video/video.h"

void kmain(void) {
	mem_page_init();
	// gdt_install();
	// serial_puts("GDT installed\n");
	__asm__ volatile("cli");
	video_init();
	serial_puts("Video init\n");
	lapic_default_init();
	serial_puts("LAPIC init\n");
	// acpi_init();
	// serial_puts("ACPI init\n");
	pci_build_device_tree();
	pci_debug_dump();
	pic_remap();
	idt_install();
	irq_double_fault_register();
	irq_gp_register();
	irq_page_fault_register();
	irq_keyboard_register();
	irq_timer_register();
	__asm__ volatile("sti");

	print_diagnostics();

	kshell_init();
	kshell_bga_register();
	kshell_help_register();
	kshell_julia_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();
	kshell_snake_register();
	kshell_tetris_register();

	kshell();
}

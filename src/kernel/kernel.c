#include "idt.h"
#include "irq_keyboard.h"
#include "irq_timer.h"
#include "kshell.h"
#include "kshell_bga.h"
#include "kshell_help.h"
#include "kshell_julia.h"
#include "kshell_mandelbrot.h"
#include "kshell_shutdown.h"
#include "pci.h"
#include "pic.h"
#include "serial.h"
#include "vga.h"

void pci_cb(struct pci_device *dev) {
	serial_puts("Device found\n");
}

void kmain(void) {
	__asm__ volatile("cli");
	pci_enumerate(pci_cb);
	pic_remap();
	idt_install();
	irq_keyboard_register();
	irq_timer_register();
	__asm__ volatile("sti");

	vga_init();
	vga_mode_set(&vga_mode_text_80x25);

	kshell_init();
	kshell_bga_register();
	kshell_help_register();
	kshell_julia_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();

	kshell();
}

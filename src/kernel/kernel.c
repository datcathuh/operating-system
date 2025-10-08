#include "idt.h"
#include "irq_keyboard.h"
#include "irq_timer.h"
#include "kshell.h"
#include "kshell_help.h"
#include "kshell_julia.h"
#include "kshell_mandelbrot.h"
#include "kshell_shutdown.h"
#include "pic.h"
#include "vga.h"

void kmain(void) {
	__asm__ volatile("cli");
	pic_remap();
	idt_install();
	irq_keyboard_register();
	irq_timer_register();
	__asm__ volatile("sti");

	vga_init();
	vga_mode_set(&vga_mode_text_80x25);

	kshell_init();
	kshell_help_register();
	kshell_julia_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();

	kshell();
}

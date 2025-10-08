#include "kshell.h"
#include "kshell_help.h"
#include "kshell_mandelbrot.h"
#include "kshell_shutdown.h"
#include "serial.h"
#include "vga.h"

void kmain(void) {
	vga_init();
	vga_mode_set(&vga_mode_text_80x25);

	kshell_init();
	kshell_help_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();
	kshell();
}

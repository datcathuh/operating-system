#include "kshell.h"
#include "kshell_help.h"
#include "kshell_mandelbrot.h"

void kmain(void) {
	kshell_init();

	kshell_help_register();
	kshell_mandelbrot_register();
	kshell();
}

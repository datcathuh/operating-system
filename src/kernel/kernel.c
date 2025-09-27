#include "kshell.h"
#include "kshell_help.h"

void kmain(void) {
	kshell_init();

	kshell_help_register();
	kshell();
}

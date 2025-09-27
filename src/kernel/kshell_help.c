#include "kshell.h"
#include "kshell_help.h"
#include "vga.h"

static void kshell_help_cb() {
	vga_put_string("Hello from this application!");
}

void kshell_help_register() {
	struct kshell_command cmd = {
		.name = "help",
		.callback = kshell_help_cb
	};
	kshell_register_command(&cmd);
}

#include "kshell.h"
#include "kshell_help.h"
#include "string.h"
#include "video/vga.h"

static void kshell_help_cb() {
	vga_put_string("Welcome to Hugo OS!\n\n");
	vga_put_string("The following commands are available:\n\n");

	const struct kshell_command* commands = kshell_commands_get();
	for(int i=0;str_length(commands[i].name) > 0;i++) {
		vga_put_string("  ");
		vga_put_string(commands[i].name);
		vga_put_string("\n");
	}
}

void kshell_help_register() {
	struct kshell_command cmd = {
		.name = "help",
		.callback = kshell_help_cb
	};
	kshell_register_command(&cmd);
}

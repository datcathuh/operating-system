#include "kshell.h"
#include "kshell_help.h"
#include "string.h"
#include "video/vga.h"

static void kshell_help_cb() {
	struct video_device *device = video_current();
	struct terminal *terminal = device->terminal;
	terminal->print(terminal, "Welcome to Hugo OS!\n\n");
	terminal->print(terminal, "The following commands are available:\n\n");

	const struct kshell_command *commands = kshell_commands_get();
	for (int i = 0; str_length(commands[i].name) > 0; i++) {
		terminal->print(terminal, "  ");
		terminal->print(terminal, commands[i].name);
		terminal->print(terminal, "\n");
	}
}

void kshell_help_register() {
	struct kshell_command cmd = {.name = "help", .callback = kshell_help_cb};
	kshell_register_command(&cmd);
}

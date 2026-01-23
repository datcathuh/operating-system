#include "keyboard.h"
#include "kshell.h"
#include "memory.h"
#include "scheduler.h"
#include "string.h"
#include "video/vga.h"

static char *_welcome =
	"  ___ ___                       ________    _________\n"
	" /   |   \\ __ __  ____   ____   \\_____  \\  /   _____/\n"
	"/    ~    \\  |  \\/ ___\\ /  _ \\   /   |   \\ \\_____  \\ \n"
	"\\    Y    /  |  / /_/  >  <_> ) /    |    \\/        \\\n"
	" \\___|_  /|____/\\___  / \\____/  \\_______  /_______  /\n"
	"       \\/      /_____/                  \\/        \\/\n";

static const char *_prompt = " > ";
#define _commands_size 10
static struct kshell_command _commands[_commands_size];

static void prompt(struct terminal *terminal) {
	int width, height;
	terminal->size(terminal, &width, &height);
	terminal->pos_set(terminal, 0, height - 1);
	terminal->print(terminal, _prompt);
	terminal->pos_set(terminal, str_length(_prompt), height - 1);
}

void kshell_init() { mem_set(_commands, 0, sizeof(_commands)); }

bool kshell_register_command(const struct kshell_command *cmd) {
	for (int i = 0; i < _commands_size; i++) {
		if (str_length(_commands[i].name) == 0) {
			mem_copy(&_commands[i], cmd, sizeof(struct kshell_command));
			return true;
		}
	}
	return false;
}

const struct kshell_command *kshell_commands_get() { return _commands; }

struct kshell_command *kshell_find_command(const char *name) {
	for (int i = 0; i < _commands_size; i++) {
		if (str_compare(_commands[i].name, name) == 0) {
			return &_commands[i];
		}
	}
	return NULL;
}

void kshell() {
	struct video_device *device = video_current();
	struct terminal *terminal = device->terminal;

	terminal->clear(terminal);
	terminal->print(terminal, _welcome);

	prompt(terminal);

	char command[70] = "";
	const int command_size = sizeof(command);

	while (1) {
		char c;
		if (!keyboard_get_key_if_exists(&c)) {
			yield();
			continue;
		}

		if (c) {
			if (c == '\n') {
				terminal->clear(terminal);

				if (str_length(command) > 0) {
					struct kshell_command *cmd = kshell_find_command(command);
					if (cmd) {
						terminal->pos_set(terminal, 0, 0);
						cmd->callback();
					} else {
						terminal->pos_set(terminal, 0, 0);
						terminal->color_set(terminal, terminal_red,
						                    terminal_black);
						terminal->print(terminal, "Invalid command");
						terminal->color_set(terminal, terminal_white,
						                    terminal_black);
					}
				}

				command[0] = 0;
				prompt(terminal);
				continue;
			}
			if (c == '\b') {
				int x, y;
				terminal->pos_get(terminal, &x, &y);
				if (x <= str_length(_prompt)) {
					continue;
				}
				command[str_length(command) - 1] = 0;
				terminal->print_char(terminal, c);
				terminal->pos_set(terminal, x - 1, y);
				continue;
			}

			if (str_append_char(command, c, command_size)) {
				terminal->print_char(terminal, c);
				int x, y;
				terminal->pos_get(terminal, &x, &y);
				terminal->pos_set(terminal, x, y);
			}
		}
	}
}

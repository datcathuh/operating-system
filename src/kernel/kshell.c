#include "keyboard.h"
#include "kshell.h"
#include "memory.h"
#include "string.h"
#include "vga.h"

static const char* _prompt = " > ";
#define _commands_size 10
static struct kshell_command _commands[_commands_size];

static void prompt() {
	vga_output_pos_set(0, vga_mode_current->height - 1);
	vga_put_string(_prompt);
	vga_cursor_pos_set(str_length(_prompt), vga_mode_current->height - 1);
}

void kshell_init() {
	mem_set(_commands, 0, sizeof(_commands));
}

bool kshell_register_command(const struct kshell_command *cmd) {
	for(int i=0;i < _commands_size;i++) {
		if(str_length(_commands[i].name) == 0) {
			mem_copy(&_commands[i], cmd, sizeof(struct kshell_command));
			return true;
		}
	}
	return false;
}

const struct kshell_command* kshell_commands_get() {
	return _commands;
}

struct kshell_command* kshell_find_command(const char *name) {
	for(int i=0;i < _commands_size;i++) {
		if(str_compare(_commands[i].name, name) == 0) {
			return &_commands[i];
		}
	}
	return NULL;
}

void kshell() {
    vga_clear();
    vga_put_string("Hello from os!\n");
    vga_put_string("t.me/x3ghx \n");
    vga_put_string_color("color support!\n", White, Magenta);

    prompt();

	char command[70] = "";
	const int command_size = sizeof(command);

    while (1) {
        char c = keyboard_get_key();
        if (c) {
			if(c == '\n') {
				vga_clear();

				if(str_length(command) > 0) {
					struct kshell_command* cmd = kshell_find_command(command);
					if(cmd) {
						vga_output_pos_set(0, 0);
						cmd->callback();
					}
					else {
						vga_output_pos_set(0, 0);
						vga_put_string_color("Invalid command", Red, Black);
					}
				}

				command[0] = 0;
				prompt();
				continue;
			}
			if(c == '\b') {
				int x,y;
				vga_output_pos_get(&x, &y);
				if(x <= str_length(_prompt)) {
					continue;
				}
				command[str_length(command) - 1] = 0;
				vga_put_char(c);
				vga_output_pos_get(&x, &y);
				vga_cursor_pos_set(x, y);
				continue;
			}

			if(str_append_char(command, c, command_size)) {
				vga_put_char(c);
				int x,y;
				vga_output_pos_get(&x, &y);
				vga_cursor_pos_set(x, y);
			}
        }
    }
}

#include "keyboard.h"
#include "kshell.h"
#include "string.h"
#include "vga.h"

static const char* _prompt = " > ";
static const int _prompt_length = sizeof(_prompt) - 1;

static void prompt() {
	vga_output_pos_set(0, 24);
	vga_put_string(_prompt);
	vga_cursor_pos_set(_prompt_length, 24);
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
				command[0] = 0;
				prompt();
				continue;
			}
			if(c == '\b') {
				int x,y;
				vga_output_pos_get(&x, &y);
				if(x <= _prompt_length) {
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

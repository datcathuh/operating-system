#include "vga.h"
#include "keyboard.h"

static void prompt() {
	vga_output_pos_set(0, 24);
	vga_put_string(" > ");
	vga_cursor_pos_set(3, 24);
}

void kmain(void) {
    vga_clear();
    vga_put_string("Hello from os!\n");
    vga_put_string("t.me/x3ghx \n");
    vga_put_string_color("color support!\n", White, Magenta);
    vga_put_string("Type something:\n> ");

	vga_output_pos_set(0, 24);
	vga_put_string(" > ");
	vga_cursor_pos_set(3, 24);

    while (1) {
        char c = keyboard_get_key();
        if (c) {
			if(c == '\n') {
				vga_clear();
				prompt();
				continue;
			}
			if(c == '\b') {
				int x,y;
				vga_output_pos_get(&x, &y);
				if(x <= 3) {
					continue;
				}
			}

            vga_put_char(c);
			int x,y;
			vga_output_pos_get(&x, &y);
			vga_cursor_pos_set(x, y);
        }
    }
}

#include "vga.h"
#include "keyboard.h"

void kmain(void) {
    vga_clear();
    vga_put_string("Hello from os!\n");
    vga_put_string("t.me/x3ghx \n");
    vga_put_string("Type something:\n> ");
    vga_put_string_color("color support!\n", VGA_COLOR_WHITE, VGA_COLOR_RED);


    while (1) {
        char c = keyboard_get_key();
        if (c) {
            vga_put_char(c);
        }
    }
}

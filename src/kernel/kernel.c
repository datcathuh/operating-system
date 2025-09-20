#include "vga.h"
#include "keyboard.h"

void kmain(void) {
    vga_clear();
    vga_put_string("Hello from os!\n");
    vga_put_string("t.me/x3ghx \n");
    vga_put_string_color("color support!\n", White, Magenta);
    vga_put_string("Type something:\n> ");


    while (1) {
        char c = keyboard_get_key();
        if (c) {
            vga_put_char(c);
        }
    }
}

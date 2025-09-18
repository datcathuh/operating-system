#include "vga.h"

static volatile uint8_t *vidmem = (uint8_t*)0xb8000;
static uint32_t cursor = 0;

static void vga_scroll(void) {
    for (uint32_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; ++i) {
        vidmem[i] = vidmem[i + VGA_WIDTH * 2];
    }
    for (uint32_t i = (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i < VGA_HEIGHT * VGA_WIDTH * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = 0x07;
    }
    cursor = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
}

void vga_clear(void) {
    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = 0x07;
    }
    cursor = 0;
}

void vga_put_char(char c) {
    if (c == '\n') {
        cursor = ((cursor / 2) / VGA_WIDTH + 1) * VGA_WIDTH * 2;
        if ((cursor / 2) / VGA_WIDTH >= VGA_HEIGHT) vga_scroll();
        return;
    }
    if (c == '\b') {
        if (cursor >= 2) cursor -= 2;
        vidmem[cursor] = ' ';
        vidmem[cursor+1] = 0x07;
        return;
    }
    vidmem[cursor] = c;
    vidmem[cursor+1] = 0x07;
    cursor += 2;
    if ((cursor / 2) % VGA_WIDTH == 0) {
        vga_put_char('\n');
    }
}

void vga_put_string(const char *s) {
    while (*s) {
        vga_put_char(*s++);
    }
}

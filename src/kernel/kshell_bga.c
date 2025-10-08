#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "kshell.h"
#include "vga.h"

// BGA I/O port definitions
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID       0x0
#define VBE_DISPI_INDEX_XRES     0x1
#define VBE_DISPI_INDEX_YRES     0x2
#define VBE_DISPI_INDEX_BPP      0x3
#define VBE_DISPI_INDEX_ENABLE   0x4

#define VBE_DISPI_DISABLED       0x00
#define VBE_DISPI_ENABLED        0x01
#define VBE_DISPI_LFB_ENABLED    0x40

#define VBE_DISPI_ID5            0xB0C5

#define BGA_FRAMEBUFFER 0xfd000000

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" :: "a"(val), "Nd"(port));
}
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// BGA helpers
static inline void bga_write(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}
static inline uint16_t bga_read(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

bool bga_is_available(void) {
    uint16_t id = bga_read(VBE_DISPI_INDEX_ID);
    return (id >= 0xB0C0 && id <= 0xB0C5);
}

void bga_set_mode(uint16_t width, uint16_t height, uint16_t bpp) {
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write(VBE_DISPI_INDEX_XRES, width);
    bga_write(VBE_DISPI_INDEX_YRES, height);
    bga_write(VBE_DISPI_INDEX_BPP, bpp);
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

static void init_graphics(void) {
    if (!bga_is_available()) {
        // No BGA detected
        return;
    }

    // Set 1024x768x32 mode

	uint32_t width = 1024, height = 768;
    bga_set_mode(width, height, 32);

    // Draw something
    volatile uint32_t* fb = (volatile uint32_t*)BGA_FRAMEBUFFER;
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            fb[y * width + x] = (x ^ y) | 0xFF000000; // pattern with alpha
        }
    }

	while(keyboard_get_key() == 0){}

    vga_mode_set(vga_mode_current);
}

static void kshell_bga_cb(void) {
	init_graphics();
}

void kshell_bga_register() {
	struct kshell_command cmd = {
		.name = "bga",
		.callback = kshell_bga_cb
	};
	kshell_register_command(&cmd);
}

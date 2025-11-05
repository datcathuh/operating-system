#include "io.h"
#include "memory.h"
#include "serial.h"
#include "vga.h"

enum vga_mode_flags {
	vga_mode_text,
	vga_mode_gfx,
};

struct vga_mode {
	char name[32];
	uint8_t misc;
	uint8_t seq[5];
	uint8_t crtc[25];
	uint8_t gfx[9];
	uint8_t attr[21];

	uint32_t width, height;
	enum vga_mode_flags flags;
};

static uint32_t cursor = 0;
static uint8_t current_color = 0x07; // Default: light gray on black
#define VGA_FONT_SIZE 4096
static uint8_t _vga_font[VGA_FONT_SIZE];

struct vga_mode vga_mode_text_80x25 = {
	.name = "80x25 (text)",
	.misc = 0x67,
	.seq = {0x03, 0x00, 0x03, 0x00, 0x02},
	.crtc = {
		0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
		0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0xF0,
		0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
		0xFF
	},
	.gfx = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x0F,
		0xFF
	},
	.attr = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x0C, 0x00, 0x0F, 0x08, 0x00
	},
	.width = 80,
	.height = 25,
	.flags = vga_mode_text,
};

struct vga_mode vga_mode_320x200x256 = {
	.name = "320x200",
	.misc = 0x63,
	.seq = {
		0x03, /* 0: Reset/Run */
		0x01, /* 1: Clocking mode */
		0x0F, /* 2: Map mask */
		0x00, /* 3: Character map select */
		0x0E  /* 4: Memory mode */
	},
	.crtc = {
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
		0xFF
	},
	.gfx = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF
	},
	.attr = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00,	0x00
	},
	.width = 320,
	.height = 200,
	.flags = vga_mode_gfx,
};

void vga_init(void) { vga_font_save(_vga_font); }

uint8_t *vga_font() {
	return _vga_font;
}

void vga_font_save(uint8_t *buffer) {
	io_outb(0x3C4, 0x02); io_outb(0x3C5, 0x04); // Write plane 2
    io_outb(0x3C4, 0x04); io_outb(0x3C5, 0x07); // Disable chain-4, odd/even

    io_outb(0x3CE, 0x04); io_outb(0x3CF, 0x02); // Read plane 2
    io_outb(0x3CE, 0x05); io_outb(0x3CF, 0x00); // Disable odd/even
    io_outb(0x3CE, 0x06); io_outb(0x3CF, 0x00); // Map A0000h

    volatile uint8_t *vga = (uint8_t *)0xA0000;
    for (int i = 0; i < 256; i++) {
        for (int y = 0; y < 16; y++) {
            buffer[i * 16 + y] = vga[i * 32 + y];

            // Debug: dump each byte as binary
			/*
            uint8_t value = buffer[i * 16 + y];
            for (int b = 7; b >= 0; b--) {
                serial_putc((value & (1 << b)) ? '1' : '0');
            }
            serial_putc('\n');
			*/
        }
    }

	io_outb(0x3C4, 0x02); io_outb(0x3C5, 0x03); // Planes 0+1
    io_outb(0x3C4, 0x04); io_outb(0x3C5, 0x03); // Enable odd/even

    io_outb(0x3CE, 0x04); io_outb(0x3CF, 0x00); // Read plane 0
    io_outb(0x3CE, 0x05); io_outb(0x3CF, 0x10); // Enable odd/even
    io_outb(0x3CE, 0x06); io_outb(0x3CF, 0x0E); // Map B8000h (text)
}

void vga_font_restore(uint8_t *font) {
    io_outw(0x3C4, 0x0100);  // reset sequencer
    io_outw(0x3C4, 0x0300);  // end reset

    io_outb(0x3C4, 0x02); io_outb(0x3C5, 0x04);   // map mask: plane 2
    io_outb(0x3C4, 0x04); io_outb(0x3C5, 0x07);   // disable chain-4 and odd/even

    io_outb(0x3CE, 0x04); io_outb(0x3CF, 0x02);   // read plane 2
    io_outb(0x3CE, 0x05); io_outb(0x3CF, 0x00);   // disable odd/even + shift
    io_outb(0x3CE, 0x06); io_outb(0x3CF, 0x00);   // map A0000-AFFFF

    volatile uint8_t *vga = (uint8_t *)0xA0000;
    for (int i = 0; i < 256; i++) {
        for (int y = 0; y < 16; y++)
            vga[i * 32 + y] = font[i * 16 + y];
    }

    io_outb(0x3C4, 0x02); io_outb(0x3C5, 0x03);   // planes 0+1
    io_outb(0x3C4, 0x04); io_outb(0x3C5, 0x03);   // re-enable chain 4
    io_outb(0x3CE, 0x04); io_outb(0x3CF, 0x00);   // read plane 0
    io_outb(0x3CE, 0x05); io_outb(0x3CF, 0x10);   // enable odd/even
    io_outb(0x3CE, 0x06); io_outb(0x3CF, 0x0E);   // map B8000-BFFFF

    io_outb(0x3D4, 0x09); io_outb(0x3D5, 0x0F);   // 16 scanlines/char
}

static inline uint8_t vga_color_entry(enum terminal_color fg, enum terminal_color bg) {
    return fg | (bg << 4);
}

static void vga_pos_set(struct terminal *terminal, int x, int y) {
	struct video_device *device = terminal->data;
    uint16_t pos = y * device->resolution->width + x;

	// Low byte of cursor position
    io_outb(0x3D4, 0x0F);
    io_outb(0x3D5, (uint8_t)(pos & 0xFF));
	// High byte of cursor position
    io_outb(0x3D4, 0x0E);
    io_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));

    cursor = x * 2 + device->resolution->width * y * 2;
}

static void vga_size(struct terminal *terminal, int *width, int *height) {
	struct video_device *device = terminal->data;
	*width = device->resolution->width;
	*height = device->resolution->height;
}

static void vga_pos_get(struct terminal *terminal, int *x, int *y) {
	struct video_device *device = terminal->data;
	*y = cursor / (device->resolution->width * 2);
	*x = (cursor % (device->resolution->width * 2)) / 2;
}

static void vga_color_get(struct terminal */*terminal*/, enum terminal_color *fg, enum terminal_color *bg) {
	*fg = current_color & 0xf0;
	*bg = current_color >> 4;
}

static void vga_color_set(struct terminal */*terminal*/, enum terminal_color fg, enum terminal_color bg) {
    current_color = vga_color_entry(fg, bg);
}

static void vga_scroll(struct video_device *device) {
    for (uint32_t i = 0; i < ((uint32_t)device->resolution->height - 1) * (uint32_t)device->resolution->width * 2; ++i) {
        device->buffer->memory[i] = device->buffer->memory[i + device->resolution->width * 2];
    }
    for (uint32_t i = (device->resolution->height - 1) * device->resolution->width * 2; i < device->resolution->height * device->resolution->width * 2; i += 2) {
        device->buffer->memory[i] = ' ';
        device->buffer->memory[i+1] = current_color;
    }
    cursor = (device->resolution->height - 1) * device->resolution->width * 2;
}

static void vga_clear(struct terminal *terminal) {
	struct video_device *device = terminal->data;
    for (uint32_t i = 0; i < device->resolution->width * device->resolution->height * 2; i += 2) {
        device->buffer->memory[i] = ' ';
        device->buffer->memory[i+1] = current_color;
    }
    cursor = 0;
}

static void vga_print_char(struct terminal *terminal, char c) {
	struct video_device *device = terminal->data;
    if (c == '\n') {
        cursor = ((cursor / 2) / device->resolution->width + 1) * device->resolution->width * 2;
        if ((cursor / 2) / device->resolution->width >= device->resolution->height) vga_scroll(device);
        return;
    }
    if (c == '\b') {
        if (cursor >= 2) cursor -= 2;
        device->buffer->memory[cursor] = ' ';
        device->buffer->memory[cursor+1] = current_color;
        return;
    }
    device->buffer->memory[cursor] = c;
    device->buffer->memory[cursor+1] = current_color;
    cursor += 2;
    if ((cursor / 2) % device->resolution->width == 0) {
        terminal->print_char(terminal, '\n');
    }
}

static void vga_print(struct terminal *terminal, const char *s) {
    while (*s) {
        terminal->print_char(terminal, *s++);
    }
}

void vga_dump_regs(void) {
    serial_puts("MISC: ");
    uint8_t m = io_inb(0x3CC);
    serial_put_hex8(m);

    serial_puts("\nSEQ:");
    for (int i=0;i<5;++i) {
        uint8_t v = io_read_indexed(0x3C4, 0x3C5, (uint8_t)i);
        serial_putc(' ');
        serial_put_hex8(v);
    }

    serial_puts("\nCRTC0-24:");
    for (int i=0;i<25;++i) {
        uint8_t v = io_read_indexed(0x3D4, 0x3D5, (uint8_t)i);
        serial_putc(' ');
        serial_put_hex8(v);
    }

    serial_puts("\nGFX:");
    for (int i=0;i<9;++i) {
        uint8_t v = io_read_indexed(0x3CE, 0x3CF, (uint8_t)i);
        serial_putc(' ');
        serial_put_hex8(v);
    }

    serial_puts("\n");
}

static void vga_display_disable(void) {
    /* Sequencer index 0 = Reset. Writing 0x01 puts in reset */
    io_outb(0x3C4, 0x00);
    io_outb(0x3C5, 0x01);
}

static void vga_display_enable(void) {
    /* Sequencer index 0 = Reset. Writing 0x03 brings out of reset/run */
    io_outb(0x3C4, 0x00);
    io_outb(0x3C5, 0x03);
}

static void vga_dac_set_entry(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    io_outb(0x3C8, index); /* start index */
    io_outb(0x3C9, r);
    io_outb(0x3C9, g);
    io_outb(0x3C9, b);
}

static void vga_dac_greyscale_palette(void) {
    for (int i = 0; i < 256; ++i) {
        uint8_t v = (uint8_t)((i * 63) / 255); /* scale 0..255 -> 0..63 */
        vga_dac_set_entry((uint8_t)i, v, v, v);
    }
}

static inline void vga_write_indexed(uint16_t idx_port, uint16_t data_port, uint8_t idx, uint8_t val) {
    io_outb(idx_port, idx);
    io_outb(data_port, val);
}

static void vga_write_attr(uint8_t index, uint8_t value) {
  /* Attribute controller write (must read 0x3DA to unlock flip-flop) */
    (void)io_inb(0x3DA);         /* reset attribute controller flip-flop */
    io_outb(0x3C0, index);
    io_outb(0x3C0, value);
}

// https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
// https://wiki.osdev.org/VGA_Hardware

static inline void pokeb(uint16_t seg, uint16_t off, uint8_t val) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	volatile uint8_t *p = (volatile uint8_t *)(uintptr_t)((seg << 4) + off);
    *p = val;
#pragma GCC diagnostic pop
}

static inline void pokew(uint16_t seg, uint16_t off, uint16_t val) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	volatile uint16_t *p = (volatile uint16_t *)(uintptr_t)((seg << 4) + off);
    *p = val;
#pragma GCC diagnostic pop
}

// Restore the standard BIOS 16-color palette for text mode
static void vga_restore_default_text_colors(void) {
    // Each color uses 6 bits per channel (0–63)
    static const uint8_t vga_default_palette[16][3] = {
        { 0,  0,  0},   // 0: black
        { 0,  0, 42},   // 1: blue
        { 0, 42,  0},   // 2: green
        { 0, 42, 42},   // 3: cyan
        {42,  0,  0},   // 4: red
        {42,  0, 42},   // 5: magenta
        {42, 21,  0},   // 6: brown
        {42, 42, 42},   // 7: light gray
        {21, 21, 21},   // 8: dark gray
        {21, 21, 63},   // 9: light blue
        {21, 63, 21},   // 10: light green
        {21, 63, 63},   // 11: light cyan
        {63, 21, 21},   // 12: light red
        {63, 21, 63},   // 13: light magenta
        {63, 63, 21},   // 14: yellow
        {63, 63, 63},   // 15: white
    };

    // Tell VGA DAC we’re updating from index 0
    io_outb(0x3C8, 0x00);

    // Write RGB triplets for colors 0–15
    for (int i = 0; i < 16; ++i) {
        io_outb(0x3C9, vga_default_palette[i][0]);
        io_outb(0x3C9, vga_default_palette[i][1]);
        io_outb(0x3C9, vga_default_palette[i][2]);
    }
}

static void vga_mode_set(struct vga_mode *mode) {
    vga_display_disable();

    /* Misc output */
    io_outb(0x3C2, mode->misc);

    /* Sequencer registers (0..4) */
    for (int i = 0; i < 5; ++i) {
		vga_write_indexed(0x3C4, 0x3C5, (uint8_t)i, mode->seq[i]);
	}

    /* Unlock and write CRTC registers (0..24) */
	io_outb(0x3D4, 0x11);
	io_outb(0x3D5, io_inb(0x3D5) & ~0x80);

    for (int i = 0; i < 25; ++i) {
		vga_write_indexed(0x3D4, 0x3D5, (uint8_t)i, mode->crtc[i]);
	}

    /* Graphics controller 0..8 */
    for (int i = 0; i < 9; ++i) {
		vga_write_indexed(0x3CE, 0x3CF, (uint8_t)i, mode->gfx[i]);
	}

    /* Attribute controller 0..20 */
    for (int i = 0; i < 21; ++i) {
		vga_write_attr((uint8_t)i, mode->attr[i]);
	}

	(void)io_inb(0x3DA);     // reset flip-flop
	io_outb(0x3C0, 0x20);

    /* Clear VGA memory at 0xA0000 (320*200 bytes) */
	if(mode->flags == vga_mode_gfx) {
		volatile uint8_t *vga = (uint8_t*)0xA0000;
		for (uint32_t i = 0; i < mode->width*mode->height; ++i) {
			vga[i] = 0x00;
		}
	}

	if(mode->flags == vga_mode_text) {
		vga_font_restore(_vga_font);

		vga_restore_default_text_colors();

		uint8_t ht = 16;
		pokew(0x40, 0x4A, mode->width);	/* columns on screen */
		pokew(0x40, 0x4C, mode->width * mode->height * 2); /* framebuffer size */
		pokew(0x40, 0x50, 0);		/* cursor pos'n */
		pokeb(0x40, 0x60, ht - 1);	/* cursor shape */
		pokeb(0x40, 0x61, ht - 2);
		pokeb(0x40, 0x84, mode->height - 1);	/* rows on screen - 1 */
		pokeb(0x40, 0x85, ht);		/* char height */
		pokew(0x40, 0x4E, 0xB800);
	}

    vga_display_enable();
}

struct video_resolution _vga_resolution;
struct terminal _terminal_vga = {
	.size = vga_size,
	.color_get = vga_color_get,
	.color_set = vga_color_set,
	.pos_get = vga_pos_get,
	.pos_set = vga_pos_set,
	.clear = vga_clear,
	.print = vga_print,
	.print_char = vga_print_char
};

static bool vga_device_initialize(struct video_device*) {
	return true;
}

static bool vga_device_resolution_set(struct video_device* device, struct video_resolution *res) {
	if(res->width == 80 && res->height == 25 && res->bpp == 4) {
		vga_mode_set(&vga_mode_text_80x25);
		mem_copy(&_vga_resolution, res, sizeof(struct video_resolution));
		device->resolution = &_vga_resolution;
		device->buffer->memory = (uint8_t*)0xb8000;

		device->terminal = &_terminal_vga;
		_terminal_vga.data = device;

		device->terminal->clear(device->terminal);
		return true;
	}
	if(res->width == 320 && res->height == 200 && res->bpp == 8) {
		vga_mode_set(&vga_mode_320x200x256);
		vga_dac_greyscale_palette();
		mem_copy(&_vga_resolution, res, sizeof(struct video_resolution));
		device->resolution = &_vga_resolution;
		device->buffer->memory = (uint8_t*)0xA0000;
		return true;
	}
	return false;
}

static bool vga_device_cleanup(struct video_device *) { return false; }

struct video_buffer _vga_buffer;

struct video_device _vga_device = {
	.resolution = 0,
	.pci_device = 0,
	.buffer = &_vga_buffer,
	.initialize = vga_device_initialize,
	.resolution_set = vga_device_resolution_set,
	.cleanup = vga_device_cleanup
};

struct video_device *vga_device() {
	return &_vga_device;
}

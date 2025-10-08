#include "io.h"
#include "serial.h"
#include "vga.h"

static volatile uint8_t *vidmem = (uint8_t*)0xb8000;
static uint32_t cursor = 0;
static uint8_t current_color = 0x07;  // Default: light gray on black

struct vga_mode *vga_mode_current = 0;

struct vga_mode vga_mode_text_80x25 = {
	.name = "80x25 (text)",
	.misc = 0x67,
	.seq = {
		0x03, 0x00, 0x03, 0x00, 0x02
	},
	.crtc = {
		0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
		0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
		0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
		0xFF
	},
	.gfx = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
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


static inline uint8_t vga_color_entry(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}

void vga_cursor_pos_set(int x, int y) {
    uint16_t pos = y * vga_mode_current->width + x;

	// Low byte of cursor position
    io_outb(0x3D4, 0x0F);
    io_outb(0x3D5, (uint8_t)(pos & 0xFF));
	// High byte of cursor position
    io_outb(0x3D4, 0x0E);
    io_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_output_pos_get(int *x, int *y) {
	*y = cursor / (vga_mode_current->width * 2);
	*x = (cursor % (vga_mode_current->width * 2)) / 2;
}

void vga_output_pos_set(int x, int y) {
    cursor = x * 2 + vga_mode_current->width * y * 2;
}

void vga_set_color(enum vga_color fg, enum vga_color bg) {
    current_color = vga_color_entry(fg, bg);
}

static void vga_scroll(void) {
    for (uint32_t i = 0; i < (vga_mode_current->height - 1) * vga_mode_current->width * 2; ++i) {
        vidmem[i] = vidmem[i + vga_mode_current->width * 2];
    }
    for (uint32_t i = (vga_mode_current->height - 1) * vga_mode_current->width * 2; i < vga_mode_current->height * vga_mode_current->width * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = current_color;
    }
    cursor = (vga_mode_current->height - 1) * vga_mode_current->width * 2;
}

void vga_clear(void) {
    for (uint32_t i = 0; i < vga_mode_current->width * vga_mode_current->height * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = current_color;
    }
    cursor = 0;
}

void vga_put_char(char c) {
    if (c == '\n') {
        cursor = ((cursor / 2) / vga_mode_current->width + 1) * vga_mode_current->width * 2;
        if ((cursor / 2) / vga_mode_current->width >= vga_mode_current->height) vga_scroll();
        return;
    }
    if (c == '\b') {
        if (cursor >= 2) cursor -= 2;
        vidmem[cursor] = ' ';
        vidmem[cursor+1] = current_color;
        return;
    }
    vidmem[cursor] = c;
    vidmem[cursor+1] = current_color;
    cursor += 2;
    if ((cursor / 2) % vga_mode_current->width == 0) {
        vga_put_char('\n');
    }
}

void vga_put_string(const char *s) {
    while (*s) {
        vga_put_char(*s++);
    }
}

void vga_put_string_color(const char *s, enum vga_color fg, enum vga_color bg) {
    uint8_t prev_color = current_color;
    vga_set_color(fg, bg);
    vga_put_string(s);
    current_color = prev_color;
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

void vga_display_disable(void) {
    /* Sequencer index 0 = Reset. Writing 0x01 puts in reset */
    io_outb(0x3C4, 0x00);
    io_outb(0x3C5, 0x01);
}

void vga_display_enable(void) {
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

void vga_dac_greyscale_palette(void) {
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

void vga_mode_set(struct vga_mode *mode) {
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
    volatile uint8_t *vga = (uint8_t*)0xA0000;
    for (uint32_t i = 0; i < mode->width*mode->height; ++i) {
		vga[i] = 0x00;
	}

    vga_display_enable();

	vga_mode_current = mode;
}

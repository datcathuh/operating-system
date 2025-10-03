/* vga_mode13_mandelbrot.c
   Freestanding kernel code (protected mode, ring 0).
   - Programs VGA registers for Mode 13h
   - Draws Mandelbrot to 0xA0000 using fixed-point arithmetic (Q16.16)
   - Waits 10 seconds using PIT polling
*/

#include "kshell.h"
#include "kshell_mandelbrot.h"
#include "serial.h"
#include <stdint.h>

/* ---- low-level I/O ---- */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

/* read indexed port (index->data pair) */
static inline uint8_t read_indexed(uint16_t idx_port, uint16_t data_port, uint8_t idx) {
    __asm__ volatile ("outb %0, %1" : : "a"(idx), "Nd"(idx_port));
    uint8_t v;
    __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(data_port));
    return v;
}

/* read attribute register (special flip-flop) */
static uint8_t read_attr(uint8_t index) {
    (void)inb(0x3DA);             /* reset flip-flop */
    outb(0x3C0, index);
    return inb(0x3C1);
}

/* Dump a few critical registers to serial */
void dump_vga_regs(void) {
    char buf[64];
    serial_puts("MISC: ");
    uint8_t m = inb(0x3CC); // read-back misc
	serial_puts("0x");
    serial_put_hex8(m); /* print hex nibble helper omitted for brevity */
    /* print as decimal for quick check */
    serial_puts(" ");
    /* Sequencer 0..4 */
    serial_puts("\nSEQ: 0x");
    for (int i=0;i<5;++i) {
        uint8_t v = read_indexed(0x3C4, 0x3C5, (uint8_t)i);
        serial_put_hex8(v);
    }
    serial_puts("\nCRTC0-4:");
    for (int i=0;i<5;++i) {
        uint8_t v = read_indexed(0x3D4, 0x3D5, (uint8_t)i);
        serial_putc(' ');
        serial_put_hex8(v);
    }
    serial_puts("\nGFX:");
    for (int i=0;i<9;++i) {
        uint8_t v = read_indexed(0x3CE, 0x3CF, (uint8_t)i);
        serial_putc(' ');
        serial_put_hex8(v);
    }

    serial_puts("\n");
}

/* write indexed register helper for ports with index/data pairs */
static inline void write_indexed(uint16_t idx_port, uint16_t data_port, uint8_t idx, uint8_t val) {
    outb(idx_port, idx);
    outb(data_port, val);
}

/* Attribute controller write (must read 0x3DA to unlock flip-flop) */
static void write_attr(uint8_t index, uint8_t value) {
    (void)inb(0x3DA);         /* reset attribute controller flip-flop */
    outb(0x3C0, index);
    outb(0x3C0, value);
}

/* ---- VGA Mode 13 register tables (canonical) ----
   Values below are the standard mode 13 register dump used in many tutorials.
   If your VM behaves oddly you can compare with OSDev / other references.
*/
static const uint8_t misc_output = 0x63;

static const uint8_t sequencer_vals[5] = {
    0x03, /* 0: Reset/Run */
    0x01, /* 1: Clocking mode */
    0x0F, /* 2: Map mask */
    0x00, /* 3: Character map select */
    0x06  /* 4: Memory mode */
};

static const uint8_t crtc_vals[25] = {
    0x5F,0x4F,0x50,0x82,0x54,0x80,0xBF,0x1F,0x00,0x41,
    0x9C,0x8E,0x8F,0x28,0x00,0x96,0xB9,0xA3,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF
};

static const uint8_t graphics_vals[9] = {
    0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF
};

static const uint8_t attribute_vals[21] = {
    /* palette indices 0..15 */
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    /* rest of attribute controller */
    0x41,0x00,0x0F,0x00,0x00
};

/* ---- Helper: disable/enable sequencer display while updating ---- */
static void disable_display(void) {
    /* Sequencer index 0 = Reset. Writing 0x01 puts in reset */
    outb(0x3C4, 0x00);
    outb(0x3C5, 0x01);
}
static void enable_display(void) {
    /* Sequencer index 0 = Reset. Writing 0x03 brings out of reset/run */
    outb(0x3C4, 0x00);
    outb(0x3C5, 0x03);
}

/* Unlock CRTC registers (clear protect bit in index 0x11) */
static void unlock_crtc(void) {
	outb(0x3D4, 0x11);
	outb(0x3D5, inb(0x3D5) & ~0x80);
}

// https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

/* Set Mode 13 by programming VGA registers (protected-mode safe) */
void set_mode13_by_registers(void) {
    /* Recommend caller has interrupts disabled; we'll be careful here */
    disable_display();

    /* Misc output */
    outb(0x3C2, misc_output);

    /* Sequencer registers (0..4) */
    for (int i = 0; i < 5; ++i) {
		write_indexed(0x3C4, 0x3C5, (uint8_t)i, sequencer_vals[i]);
	}

    /* Unlock and write CRTC registers (0..24) */
    unlock_crtc();
    for (int i = 0; i < 25; ++i) {
		write_indexed(0x3D4, 0x3D5, (uint8_t)i, crtc_vals[i]);
	}

    /* Graphics controller 0..8 */
    for (int i = 0; i < 9; ++i) {
		write_indexed(0x3CE, 0x3CF, (uint8_t)i, graphics_vals[i]);
	}

    /* Attribute controller 0..20 */
    for (int i = 0; i < 21; ++i) {
		write_attr((uint8_t)i, attribute_vals[i]);
	}

	(void)inb(0x3DA);     // reset flip-flop
	outb(0x3C0, 0x20); 

    /* Clear VGA memory at 0xA0000 (320*200 bytes) */
    volatile uint8_t *vga = (uint8_t*)0xA0000;
    for (int i = 0; i < 320*200; ++i) {
		vga[i] = 0xf0;
	}

    enable_display();
}

/* ---- Palette (DAC) helper: set single palette entry (each component 0..63) ---- */
static void set_dac_entry(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    outb(0x3C8, index); /* start index */
    outb(0x3C9, r);
    outb(0x3C9, g);
    outb(0x3C9, b);
}

/* Load a simple greyscale palette (optional) */
static void load_greyscale_palette(void) {
    for (int i = 0; i < 256; ++i) {
        uint8_t v = (uint8_t)((i * 63) / 255); /* scale 0..255 -> 0..63 */
        set_dac_entry((uint8_t)i, v, v, v);
    }
}

/* ---- Mandelbrot using Q16.16 fixed point arithmetic ----
   - x0,y0 are Q16.16
   - x,y are Q16.16
   - use 64-bit temporaries for products
*/
static void draw_mandelbrot_mode13(void) {
    const int width = 320;
    const int height = 200;
    volatile uint8_t *frame = (uint8_t*)0xA0000;

    /* scaling factor: map pixels to [-2, +2] in X and Y */
    /* x0 = (px - width/2) * (4.0 / width) in Q16.16 */
    const int32_t q16_one = 1 << 16;
    const int64_t scale_numer = (int64_t)4 * q16_one; /* 4 in Q16.16 = 4<<16 */
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            int32_t x0 = (int32_t)(((int64_t)(px - width/2) * scale_numer) / width);
            int32_t y0 = (int32_t)(((int64_t)(py - height/2) * scale_numer) / height);

            int32_t x = 0, y = 0;
            int iteration = 0;
            const int max_iteration = 50;

            while (1) {
                /* x*x and y*y in Q16.16 after shifting back by 16 */
                int64_t xx = (int64_t)x * x;
                int64_t yy = (int64_t)y * y;
                int64_t xsq = xx >> 16;
                int64_t ysq = yy >> 16;

                if (xsq + ysq > (4LL << 16)) break; /* magnitude^2 > 4 */

                if (iteration >= max_iteration) break;

                /* xtemp = x*x - y*y + x0  (all Q16.16) */
                int32_t xtemp = (int32_t)(xsq - ysq + x0);

                /* y = 2*x*y + y0 */
                int64_t xy2 = (int64_t)x * y;
                int32_t ynew = (int32_t)((xy2 >> 15) + y0); /* 2*x*y -> (x*y<<1) which is (x*y)>>15 after >>16 adjust */
                x = xtemp;
                y = ynew;

                iteration++;
            }

            uint8_t color = (iteration == max_iteration) ? 0 : (uint8_t)((iteration * 5) & 0xFF);
            frame[py * width + px] = color;
        }
    }
}

/* ---- PIT-based polling wait (no IRQs): program PIT channel 0 to ~100Hz and poll its counter.
   Explanation:
   - We program PIT channel 0, access mode lobyte/hibyte, mode 2 (rate generator)
   - Divisor chosen to produce ~100 Hz (1193182 / 11932 ~= 100)
   - We then repeatedly latch and read the current counter and compute elapsed ticks.
*/
static void pit_wait_seconds(uint32_t seconds) {
    const uint32_t PIT_FREQ = 1193182U;
    const uint32_t TARGET_HZ = 100; /* 100 ticks per second */
    uint16_t divisor = (uint16_t)(PIT_FREQ / TARGET_HZ);
    if (divisor == 0) divisor = 1;

    /* Command: channel 0, access lobyte/hibyte (3), mode 2 (rate gen), binary (0) */
    outb(0x43, 0x34);
    outb(0x40, (uint8_t)(divisor & 0xFF));      /* low */
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF)); /* high */

    uint32_t target_ticks = seconds * TARGET_HZ;
    uint32_t ticks = 0;

    /* initial_count == divisor, but we will compute elapsed by (initial - current) mod divisor */
    uint16_t initial = divisor;

    while (ticks < target_ticks) {
        /* Latch current count for channel 0: control word with latch (access=00) */
        outb(0x43, 0x00); /* latch count for channel 0 */
        uint8_t lo = inb(0x40);
        uint8_t hi = inb(0x40);
        uint16_t current = (uint16_t)((hi << 8) | lo);
        /* In mode 2 the counter counts down; compute elapsed = (initial - current) mod 65536,
           but since divisor < 65536, and initial==divisor, we can do a small modulo */
        uint32_t elapsed;
        if (initial >= current) elapsed = (uint32_t)(initial - current);
        else elapsed = (uint32_t)(initial + (0x10000 - current));

        ticks = elapsed;
        /* busy loop — we could optionally add a small NOP to reduce CPU usage */
    }
}

static void kshell_mandelbrot_cb(void) {
    /* It's safer if caller already disabled interrupts, but we'll disable here too */
    __asm__ volatile ("cli");

    set_mode13_by_registers();
    load_greyscale_palette();    /* optional: load greyscale palette so indices map to visible shades */
    // draw_mandelbrot_mode13();


	volatile uint8_t* vga = (uint8_t*)0xA0000;
	for (int i = 0; i < 320*200; ++i)
		vga[i] = i % 256; // fill with a simple gradient

	dump_vga_regs();

    /* Optionally: return to text mode (if you want). Here we simply hang. */
    for (;;) __asm__ volatile ("hlt");
}

void kshell_mandelbrot_register() {
	struct kshell_command cmd = {
		.name = "mandelbrot",
		.callback = kshell_mandelbrot_cb
	};
	kshell_register_command(&cmd);
}

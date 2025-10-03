/* vga_mode13_mandelbrot.c
   Freestanding kernel code (protected mode, ring 0).
   - Programs VGA registers for Mode 13h
   - Draws Mandelbrot to 0xA0000 using fixed-point arithmetic (Q16.16)
   - Waits 10 seconds using PIT polling
*/

#include "io.h"
#include "kshell.h"
#include "kshell_mandelbrot.h"
#include "serial.h"
#include "vga.h"
#include <stdint.h>

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

    /* Program PIT: channel 0, lobyte/hibyte, mode 2 (rate generator) */
    io_outb(0x43, 0x34);
    io_outb(0x40, divisor & 0xFF);
    io_outb(0x40, divisor >> 8);

    uint32_t ticks_needed = seconds * TARGET_HZ;
    uint32_t ticks = 0;

    /* Read initial counter value */
    io_outb(0x43, 0x00);  // latch count
    uint16_t last = io_inb(0x40) | (io_inb(0x40) << 8);

    while (ticks < ticks_needed) {
        io_outb(0x43, 0x00);  // latch again
        uint16_t now = io_inb(0x40) | (io_inb(0x40) << 8);

        if (now > last) {
            /* Counter wrapped around once */
            ticks++;
        }
        last = now;
    }
}

static void kshell_mandelbrot_cb(void) {
    struct vga_mode *prev = vga_mode_current;

    vga_mode_set(&vga_mode_320x200x256);
    vga_dac_greyscale_palette();    /* optional: load greyscale palette so indices map to visible shades */
    draw_mandelbrot_mode13();

	pit_wait_seconds(10);

	vga_dump_regs();

    vga_mode_set(prev);
}

void kshell_mandelbrot_register() {
	struct kshell_command cmd = {
		.name = "mandelbrot",
		.callback = kshell_mandelbrot_cb
	};
	kshell_register_command(&cmd);
}

#include "kshell.h"
#include "kshell_julia.h"
#include "keyboard.h"
#include "vga.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define MAX_ITER 128

// Multiply 16.16 fixed numbers
static int fmul(int a, int b) {
    long long t = (long long)a * (long long)b;
    return (int)(t >> 16);
}

static void draw_julia(double xmin, double xmax, double ymin, double ymax) {
	int cx = -45862;    // -0.7 in 16.16 fixed point
	int cy = 17722;     // 0.27 in 16.16 fixed point

	volatile uint8_t *vga = (uint8_t*)0xA0000;
	int x, y;
    int dx = (xmax - xmin) / SCREEN_WIDTH;
    int dy = (ymax - ymin) / SCREEN_HEIGHT;

    for(y = 0; y < SCREEN_HEIGHT; y++) {
        int zy = ymin + y * dy;
        for(x = 0; x < SCREEN_WIDTH; x++) {
            int zx = xmin + x * dx;
            int i = 0;

            int zx2 = zx;
            int zy2 = zy;

            while(i < MAX_ITER) {
                int zx2_sq = fmul(zx2, zx2);
                int zy2_sq = fmul(zy2, zy2);
                if((zx2_sq + zy2_sq) > (4 << 16)) break;

                int tmp = zx2_sq - zy2_sq + cx;
                zy2 = fmul(2 * zx2, zy2) + cy;
                zx2 = tmp;

                i++;
            }
            vga[y * SCREEN_WIDTH + x] = i % 256;
        }
    }
}

static void kshell_julia_cb(void) {
    struct vga_mode *prev = vga_mode_current;

    vga_mode_set(&vga_mode_320x200x256);
    vga_dac_greyscale_palette();    /* optional: load greyscale palette so indices map to visible shades */

	int xmin = -98304;  // -1.5
	int xmax = 98304;   // 1.5
	int ymin = -65536;  // -1.0
	int ymax = 65536;   // 1.0
	int zoom_factor = 62914; // 0.95 in 16.16 fixed

	for(int i=0;i < 70; i++) {
		draw_julia(xmin, xmax, ymin, ymax);

		int xcenter = (xmin + xmax) / 2;
        int ycenter = (ymin + ymax) / 2;
        int xhalf = fmul((xmax - xmin)/2, zoom_factor);
        int yhalf = fmul((ymax - ymin)/2, zoom_factor);

        xmin = xcenter - xhalf;
        xmax = xcenter + xhalf;
        ymin = ycenter - yhalf;
        ymax = ycenter + yhalf;
	}

	while(keyboard_get_key() == 0){}

    vga_mode_set(prev);
}

void kshell_julia_register() {
	struct kshell_command cmd = {
		.name = "julia",
		.callback = kshell_julia_cb
	};
	kshell_register_command(&cmd);
}

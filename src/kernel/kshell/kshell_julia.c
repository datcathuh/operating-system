#include "kshell.h"
#include "kshell_julia.h"
#include "keyboard.h"
#include "memory.h"
#include "video/video.h"

#define MAX_ITER 128

// Multiply 16.16 fixed numbers
static int fmul(int a, int b) {
    long long t = (long long)a * (long long)b;
    return (int)(t >> 16);
}

static void draw_julia(double xmin, double xmax, double ymin, double ymax) {
	struct video_device *device = video_current();
	int cx = -45862;    // -0.7 in 16.16 fixed point
	int cy = 17722;     // 0.27 in 16.16 fixed point

	volatile uint8_t *vga = device->buffer->memory;
	int x, y;
    int dx = (xmax - xmin) / device->buffer->resolution.width;
    int dy = (ymax - ymin) / device->buffer->resolution.height;

    for(y = 0; y < device->buffer->resolution.height; y++) {
        int zy = ymin + y * dy;
        for(x = 0; x < device->buffer->resolution.width; x++) {
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
            vga[y * device->buffer->resolution.width + x] = i % 256;
        }
    }
}

static void kshell_julia_cb(void) {
	struct video_device *device = video_current();

	struct video_resolution prev;
	mem_copy(&prev, device->resolution, sizeof(struct video_resolution));

	struct video_resolution newres = {
		.width = 320,
		.height = 200,
		.bpp = 8
	};
	device->resolution_set(device, &newres);

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

	device->resolution_set(device, &prev);
}

void kshell_julia_register() {
	struct kshell_command cmd = {
		.name = "julia",
		.callback = kshell_julia_cb
	};
	kshell_register_command(&cmd);
}

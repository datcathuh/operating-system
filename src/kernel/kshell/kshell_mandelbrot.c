#include "kshell.h"
#include "kshell_mandelbrot.h"
#include "keyboard.h"
#include "memory.h"
#include "video/video.h"


/* ---- Mandelbrot using Q16.16 fixed point arithmetic ----
   - x0,y0 are Q16.16
   - x,y are Q16.16
   - use 64-bit temporaries for products
*/
static void kshell_mandelbrot_cb(void) {
	struct video_device *device = video_current();
    const int32_t width = device->buffer->resolution.width;
    const int32_t height = device->buffer->resolution.height;

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

            uint8_t color1 = (iteration == max_iteration) ? 0 : (uint8_t)((iteration * 5) & 0xFF);
			uint32_t color = color1 | color1 << 8 | color1 << 16;
			video_draw_pixel(device->buffer, px, py, color);
        }
    }

	while(keyboard_get_key() == 0){}
}

void kshell_mandelbrot_register() {
	struct kshell_command cmd = {
		.name = "mandelbrot",
		.callback = kshell_mandelbrot_cb
	};
	kshell_register_command(&cmd);
}

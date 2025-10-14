#include "io.h"
#include "keyboard.h"
#include "kshell.h"
#include "memory.h"
#include "types.h"
#include "video/bga.h"

#define BGA_FRAMEBUFFER 0xfd000000

static void kshell_bga_cb(void) {
	struct video_device *prev_dev = video_current();
	struct video_resolution prev_res;
	mem_copy(&prev_res, prev_dev->resolution, sizeof(struct video_resolution));

	struct video_device *new_dev = bga_device();
	struct video_resolution res = {
		.width = 1024,
		.height = 768,
		.bpp = 32
	};
	video_set(new_dev);
	new_dev->resolution_set(new_dev, &res);

    // Draw something
    volatile uint32_t* fb = (volatile uint32_t*)BGA_FRAMEBUFFER;
    for (uint32_t y = 0; y < res.height; y++) {
        for (uint32_t x = 0; x < res.width; x++) {
            fb[y * res.width + x] = (x ^ y) | 0xFF000000; // pattern with alpha
        }
    }

	while(keyboard_get_key() == 0){}

	video_set(prev_dev);
	prev_dev->resolution_set(prev_dev, &prev_res);
}

void kshell_bga_register() {
	struct kshell_command cmd = {
		.name = "bga",
		.callback = kshell_bga_cb
	};
	kshell_register_command(&cmd);
}

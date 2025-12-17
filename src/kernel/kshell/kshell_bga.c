#include "io.h"
#include "keyboard.h"
#include "kshell.h"
#include "memory.h"
#include "types.h"
#include "video/bga.h"
#include "video/vga.h"

static void kshell_bga_cb(void) {
	struct video_device *dev = video_current();

	for (uint32_t y = 0; y < dev->buffer->resolution.height; y++) {
		for (uint32_t x = 0; x < dev->buffer->resolution.width; x++) {
			video_draw_pixel(dev->buffer, x, y, x ^ y);
		}
	}

	video_draw_string(dev->buffer, vga_font(), 100, 100, "Hugo\nwas here!",
	                  0xffffff, 0x0, 1);
	video_draw_line(dev->buffer, 10, 10, 200, 200, 0xff0000);
	video_draw_rect(dev->buffer, 250, 10, 400, 30, 0x00ff00);
	video_draw_rect_filled(dev->buffer, 250, 50, 400, 90, 0x00ffff);

	while (keyboard_get_key() == 0) {
	}
}

void kshell_bga_register() {
	struct kshell_command cmd = {.name = "bga", .callback = kshell_bga_cb};
	kshell_register_command(&cmd);
}

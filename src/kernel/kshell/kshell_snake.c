#include "keyboard.h"
#include "kshell.h"
#include "kshell_snake.h"
#include "memory.h"
#include "pit.h"
#include "string.h"
#include "types.h"
#include "video/vga.h"
#include "video/video.h"

#define COL_BORDER 0xffffff
#define COL_BG 0x000000

static uint8_t *_font = NULL;

void kshell_snake_cb(void) {
	struct video_device *vd = video_current();
	if (!vd)
		return;

	_font = vga_font();
	if (!_font) {
		return;
	}

	struct video_buffer *buffer = video_buffer_allocate(vd, vd->resolution);

	while (true) {
		/* video_draw_rect_filled(vd, px+1, py+1, CELL_SIZE-2, CELL_SIZE-2,
		   color); video_draw_rect(vd, px, py, CELL_SIZE, CELL_SIZE,
		   GRID_COLOR);*/
		video_draw_rect_filled(buffer, 0, 0, buffer->resolution.width,
		                       buffer->resolution.height, COL_BG);
		video_draw_rect_filled(buffer, 10, 10, buffer->resolution.width - 20,
		                       buffer->resolution.height - 20, COL_BORDER);
		video_draw_rect_filled(buffer, 15, 15, buffer->resolution.width - 30,
		                       buffer->resolution.height - 30, COL_BG);
		char k;
		bool key_pressed = keyboard_get_key_if_exists(&k);
		if (key_pressed && k == 'q') {
			break;
		}
		pit_wait_milliseconds(100);
	}

	video_draw_rect_filled(buffer, 0, 0, buffer->resolution.width,
	                       buffer->resolution.height, COL_BG);
}

void kshell_snake_register() {
	struct kshell_command cmd = {.name = "snake", .callback = kshell_snake_cb};
	kshell_register_command(&cmd);
}

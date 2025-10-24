#include "memory.h"
#include "terminal_gfx.h"
#include "video/vga.h"

static int _x = 0;
static int _y = 0;
static uint32_t _col_fg = 0xffffff;
static uint32_t _col_bg = 0x000000;
static const uint8_t _default_palette[16][3] = {
    {0, 0, 0},       // 0: black
    {0, 0, 171},     // 1: blue
    {0, 171, 0},     // 2: green
    {0, 171, 171},   // 3: cyan
    {171, 0, 0},     // 4: red
    {171, 0, 171},   // 5: magenta
    {171, 85, 0},    // 6: brown
    {171, 171, 171}, // 7: light gray
    {85, 85, 85},    // 8: dark gray
    {85, 85, 255},   // 9: light blue
    {85, 255, 85},   // 10: light green
    {85, 255, 255},  // 11: light cyan
    {255, 85, 85},   // 12: light red
    {255, 85, 255},  // 13: light magenta
    {255, 255, 85},  // 14: yellow
    {255, 255, 255}, // 15: white
};

static void terminal_gfx_size(struct terminal *t, int *width, int *height) {
	struct video_device *device = t->data;
	*width = device->resolution->width / 8;
	*height = device->resolution->height / 16;
}

static void terminal_gfx_color_get(struct terminal */*t*/,
								   enum terminal_color *fg,
                                   enum terminal_color *bg) {
	for(int i=0; i < 16; i++) {
		if(_default_palette[i][0] == ((_col_fg >> 16) & 0xff) &&
		   _default_palette[i][0] == ((_col_fg >> 8) & 0xff) &&
		   _default_palette[i][0] == (_col_fg & 0xff)) {
			*fg = i;
			break;
		}
	}
	for(int i=0; i < 16; i++) {
		if(_default_palette[i][0] == ((_col_bg >> 16) & 0xff) &&
		   _default_palette[i][0] == ((_col_bg >> 8) & 0xff) &&
		   _default_palette[i][0] == (_col_bg & 0xff)) {
			*bg = i;
			break;
		}
	}
}

static void terminal_gfx_color_set(struct terminal */*t*/,
								   enum terminal_color fg,
                                   enum terminal_color bg) {
	_col_fg =
		((uint32_t)_default_palette[fg][0]) << 16 |
		((uint32_t)_default_palette[fg][1]) << 8 |
		(uint32_t)_default_palette[fg][2];
	_col_bg =
		((uint32_t)_default_palette[bg][0]) << 16 |
		((uint32_t)_default_palette[bg][1]) << 8 |
		(uint32_t)_default_palette[bg][2];
}

static void terminal_gfx_pos_set(struct terminal */*t*/, int x, int y) {
	_x = x;
	_y = y;
}

static void terminal_gfx_pos_get(struct terminal */*t*/, int *x, int *y) {
	*x = _x;
	*y = _y;
}

static void terminal_gfx_clear(struct terminal *t) {
	struct video_device *device = t->data;
	mem_set(device->vidmem, 0, device->resolution->width * device->resolution->height * device->resolution->bpp / 8);
}

static void terminal_gfx_print(struct terminal *t, const char *s) {
	int x = _x * 8;
	int y = _y * 16;
	struct video_device *device = t->data;
	video_draw_string(device, vga_font(), x, y, s, _col_fg, _col_bg, 1);
	for(int i=0;true; i++) {
		if(s[i] == 0) {
			break;
		}
		if(s[i] == '\n') {
			_x = 0;
			_y++;
		} else if(s[i] == '\b') {
			_x--;
		} else {
			_x++;
		}
	}
}

static void terminal_gfx_print_char(struct terminal *t, char c) {
	char b[2];
	b[0] = c;
	b[1] = 0;
	t->print(t, b);
}

static struct terminal _terminal = {
	.size = terminal_gfx_size,
	.color_get = terminal_gfx_color_get,
	.color_set = terminal_gfx_color_set,
	.pos_get = terminal_gfx_pos_get,
	.pos_set = terminal_gfx_pos_set,
	.clear = terminal_gfx_clear,
	.print = terminal_gfx_print,
	.print_char = terminal_gfx_print_char
};

struct terminal *terminal_gfx(struct video_device *device) {
	_terminal.data = device;
	return &_terminal;
}

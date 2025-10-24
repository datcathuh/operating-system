#pragma once

enum terminal_color {
    terminal_black = 0,
    terminal_blue = 1,
    terminal_green = 2,
    terminal_cyan = 3,
    terminal_red = 4,
    terminal_magenta = 5,
    terminal_brown = 6,
    terminal_light_grey = 7,
    terminal_dark_grey = 8,
    terminal_light_blue = 9,
    terminal_light_green = 10,
    terminal_light_cyan = 11,
    terminal_light_red = 12,
    terminal_light_magenta = 13,
    terminal_yellow = 14,
    terminal_white = 15,
};

struct terminal {
	void (*color_get)(struct terminal *t, enum terminal_color *fg, enum terminal_color *bg);
	void (*color_set)(struct terminal *t, enum terminal_color fg, enum terminal_color bg);
	void (*pos_set)(struct terminal *t, int x, int y);
	void (*pos_get)(struct terminal *t, int *x, int *y);
	void (*clear)(struct terminal *t);
	void (*print)(struct terminal *t, const char *s);
	void (*print_char)(struct terminal *t, char c);

	void *data;
};

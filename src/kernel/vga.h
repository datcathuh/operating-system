#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// colors
enum vga_color {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGrey = 7,
    DarkGrey = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    Yellow = 14,
    White = 15,
};

void vga_cursor_pos_set(int x, int y);
void vga_output_pos_get(int *x, int *y);
void vga_output_pos_set(int x, int y);
void vga_clear(void);
void vga_put_char(char c);
void vga_put_string(const char *s);
void vga_set_color(enum vga_color fg, enum vga_color bg);
void vga_put_string_color(const char *s, enum vga_color fg, enum vga_color bg);

struct vga_mode {
	char name[32];
	uint8_t misc;
	uint8_t seq[5];
	uint8_t crtc[25];
	uint8_t gfx[9];

	int width, height;
};

void vga_dump_regs(void);
void vga_display_disable(void);
void vga_display_enable(void);
void vga_dac_greyscale_palette(void);
void vga_mode_set(struct vga_mode *mode);

extern struct vga_mode vga_mode_320x200;

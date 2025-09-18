
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void vga_clear(void);
void vga_put_char(char c);
void vga_put_string(const char *s);


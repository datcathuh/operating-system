#include "types.h"
#include "video.h"

void vga_init(void);
void vga_font_save(uint8_t *buffer);
void vga_font_restore(uint8_t *buffer);
uint8_t *vga_font();

void vga_dump_regs(void);

struct video_device *vga_device();

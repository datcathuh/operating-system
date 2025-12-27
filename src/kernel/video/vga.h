#include "types.h"
#include "video.h"

void vga_init(void);

void vga_dump_regs(void);

struct video_device *vga_device();

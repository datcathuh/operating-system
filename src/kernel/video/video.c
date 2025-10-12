#include "vga.h"
#include "video.h"

static struct video_device *_current = 0;

void video_init() {
	vga_init();

	/* During boot of a computer we set the default device
	   to be VGA with 80x25. */
	_current = vga_device();
	struct video_resolution res = {
		.width = 80,
		.height = 24,
		.bpp = 4
	};
	_current->initialize(_current);
	_current->resolution_set(_current, &res);
}

struct video_device *video_current() {
	return _current;
}

bool video_set(struct video_device* device) {
	return false;
}

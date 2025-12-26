#include "memory.h"
#include "video/gop.h"

static bool gop_device_initialize(struct video_device *) {
	return true;
}

static bool gop_device_resolution_set(struct video_device *,
                                      struct video_resolution *) {
	/* We cannot change this. UEFI has setup everything and
	   we have no means of communicating any change. */
	return false;
}


static bool gop_device_cleanup(struct video_device *) {
	return true;
}

static struct video_buffer _video_gop_buffer;

static struct video_device _gop_device = {.resolution = 0,
										  .buffer = &_video_gop_buffer,
										  .initialize = gop_device_initialize,
										  .resolution_set = gop_device_resolution_set,
										  .cleanup = gop_device_cleanup};

struct video_device *gop_device(uint8_t *memory, struct video_resolution *resolution) {
	_video_gop_buffer.memory = memory;
	mem_copy(&_video_gop_buffer.resolution, resolution, sizeof(struct video_resolution));
	return &_gop_device;
}

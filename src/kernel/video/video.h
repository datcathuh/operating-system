#pragma once

#include "types.h"
#include "pci.h"

struct video_resolution {
	uint16_t width, height;

	/* Bits per pixel */
	uint8_t bpp;
};

struct video_device {
	struct video_resolution *resolution;

	bool (*initialize)(struct video_device*);
	bool (*resolution_set)(struct video_device*, struct video_resolution *);
	bool (*cleanup)(struct video_device*);
};

void video_init();
struct video_device *video_current();
bool video_set(struct video_device* device);

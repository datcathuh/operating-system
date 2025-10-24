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
	struct pci_device *pci_device;
	uint8_t *vidmem;

	bool (*initialize)(struct video_device*);
	bool (*resolution_set)(struct video_device*, struct video_resolution *);
	bool (*cleanup)(struct video_device*);
};

void video_init();
struct video_device *video_current();
bool video_set(struct video_device* device);

void video_draw_string(struct video_device *device,
					   const uint8_t *font,
					   int px, int py,
					   const char *s,
					   uint32_t fg_color,
					   uint32_t bg_color,
					   int scale);

#pragma once

#include "terminal.h"
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
	struct terminal *terminal;

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
void video_draw_line(struct video_device *device, int x0, int y0, int x1,
                     int y1, uint32_t color);
void video_draw_rect(struct video_device *device,
					 int x, int y, int width, int height,
					 uint32_t color);
void video_draw_rect_filled(struct video_device *device,
							int x, int y, int width, int height,
							uint32_t color);


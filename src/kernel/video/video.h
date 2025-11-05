#pragma once

#include "terminal.h"
#include "types.h"
#include "pci.h"

struct video_resolution {
	uint16_t width, height;

	/* Bits per pixel */
	uint8_t bpp;
};

struct video_buffer {
	uint8_t *memory;
	struct video_resolution resolution;
};

struct video_device {
	struct video_resolution *resolution;
	struct pci_device *pci_device;
	struct video_buffer *buffer;
	struct terminal *terminal;

	bool (*initialize)(struct video_device*);
	bool (*resolution_set)(struct video_device*, struct video_resolution *);
	bool (*cleanup)(struct video_device*);
};

void video_init();
struct video_device *video_current();
bool video_set(struct video_device* device);

void video_draw_string(struct video_buffer *buffer,
					   const uint8_t *font,
					   int px, int py,
					   const char *s,
					   uint32_t fg_color,
					   uint32_t bg_color,
					   int scale);
void video_draw_line(struct video_buffer *buffer, int x0, int y0, int x1,
                     int y1, uint32_t color);
void video_draw_rect(struct video_buffer *buffer,
					 int x, int y, int width, int height,
					 uint32_t color);
void video_draw_rect_filled(struct video_buffer *buffer, int x, int y,
                            int width, int height, uint32_t color);
inline void video_draw_pixel(struct video_buffer *buffer, int x, int y,
							 uint32_t color) {
	if(buffer->resolution.bpp != 24) {
		return;
	}
	int pitch = buffer->resolution.width * 3;
    uint8_t *fb = (uint8_t *)buffer->memory + y * pitch + x * 3;
	fb[0] = (uint8_t)(color & 0xFF);         /* B */
	fb[1] = (uint8_t)((color >> 8) & 0xFF);  /* G */
	fb[2] = (uint8_t)((color >> 16) & 0xFF); /* R */
}

struct video_buffer *video_buffer_allocate(struct video_device *device,
										   struct video_resolution *resolution);

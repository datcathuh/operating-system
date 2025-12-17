#include "math.h"
#include "memory.h"
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
		.height = 25,
		.bpp = 4
	};
	_current->initialize(_current);
	_current->resolution_set(_current, &res);
}

struct video_device *video_current() {
	return _current;
}

bool video_set(struct video_device* device) {
	if(_current) {
		_current->cleanup(_current);
		_current = 0;
	}
	_current = device;
	_current->initialize(_current);
	return true;
}



















#define FONT_WIDTH 8
#define FONT_HEIGHT 16    /* change to 8 if your font is 8x8 */

static inline uint16_t rgb_to_rgb565(uint32_t rgb)
{
    /* rgb is 0xRRGGBB */
    uint32_t r = (rgb >> 16) & 0xFF;
    uint32_t g = (rgb >> 8) & 0xFF;
    uint32_t b = (rgb) & 0xFF;
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

/* Draw a single pixel at x,y. framebuffer is a pointer to the start of the buffer.
 * pitch = bytes per scanline. bytes_per_pixel = 4 or 2.
 * color32 is 0xRRGGBB (no alpha). */
static inline void video_set_pixel(void *framebuffer, int pitch, int bytes_per_pixel,
								   int x, int y, uint32_t color32)
{
    uint8_t *fb = (uint8_t *)framebuffer + y * pitch + x * bytes_per_pixel;

    if (bytes_per_pixel == 4) {
        /* Write 32-bit pixel as 0x00RRGGBB or 0xRRGGBB (we ignore alpha) */
        uint32_t px = color32 & 0x00FFFFFFu;
        /* If your BGA expects BGRA or little-endian order, this still works on little-endian x86: px in memory will be 0xBBGGRR00
           but most systems render 0xRRGGBB when reading 32-bit as 0x00RRGGBB; adapt if needed. */
        *(uint32_t *)fb = px;
    } else if (bytes_per_pixel == 2) {
        uint16_t px = rgb_to_rgb565(color32);
        *(uint16_t *)fb = px;
    } else if (bytes_per_pixel == 3) {
        /* 24-bit: store B, G, R in memory on little-endian framebuffer */
        fb[0] = (uint8_t)(color32 & 0xFF);         /* B */
        fb[1] = (uint8_t)((color32 >> 8) & 0xFF);  /* G */
        fb[2] = (uint8_t)((color32 >> 16) & 0xFF); /* R */
    } else {
        /* Unsupported bpp; silent fail (or you can assert). */
    }
}

void video_draw_pixel(struct video_buffer *buffer, int x, int y,
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

/* Draw an 8x16 glyph at pixel coordinates (px,py). scale is integer >= 1.
 * font must point to 256 * FONT_HEIGHT bytes; glyph c starts at font[c*FONT_HEIGHT].
 * Foreground and background colors are 0xRRGGBB.
 */
void video_draw_char(void *framebuffer, int pitch, int bytes_per_pixel,
					 int screen_width, int screen_height,
					 const uint8_t *font, unsigned char c,
					 int px, int py,
					 uint32_t fg_color, uint32_t bg_color,
					 int scale)
{
    if (scale <= 0) scale = 1;

    /* Clip quickly: if whole glyph outside, return */
    int glyph_w = FONT_WIDTH * scale;
    int glyph_h = FONT_HEIGHT * scale;
    if (px + glyph_w <= 0 || py + glyph_h <= 0 || px >= screen_width || py >= screen_height)
        return;

    const uint8_t *glyph = font + (c * FONT_HEIGHT);

    for (int row = 0; row < FONT_HEIGHT; ++row) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_WIDTH; ++col) {
            int bit = (bits >> (7 - col)) & 1;
            uint32_t color = bit ? fg_color : bg_color;

            /* draw scaled pixel block (scale x scale) */
            for (int sy = 0; sy < scale; ++sy) {
                int y = py + row * scale + sy;
                if (y < 0 || y >= screen_height) continue;
                for (int sx = 0; sx < scale; ++sx) {
                    int x = px + col * scale + sx;
                    if (x < 0 || x >= screen_width) continue;
                    video_set_pixel(framebuffer, pitch, bytes_per_pixel, x, y, color);
                }
            }
        }
    }
}

void video_draw_string(struct video_buffer *buffer,
					   const uint8_t *font,
					   int px, int py,
					   const char *s,
					   uint32_t fg_color,
					   uint32_t bg_color,
					   int scale) {
	int pitch = buffer->resolution.width * buffer->resolution.bpp / 8;
    int x = px;
    int y = py;
    while (*s) {
        if (*s == '\n') {
            x = px; /* return to start */
            y += FONT_HEIGHT * scale;
            s++;
            continue;
        }
		if(*s == '\b') {
			x-= FONT_WIDTH;
			video_draw_char(
				buffer->memory,
				pitch,
				buffer->resolution.bpp / 8,
				buffer->resolution.width,
				buffer->resolution.height,
				font, ' ',
				x,
				y,
				fg_color,
				bg_color,
				scale);
            s++;
			continue;
		}
        video_draw_char(
			buffer->memory,
			pitch,
			buffer->resolution.bpp / 8,
			buffer->resolution.width,
			buffer->resolution.height,
			font, (unsigned char)*s,
			x,
			y,
			fg_color,
			bg_color,
			scale);
        x += FONT_WIDTH * scale;
        s++;
        /* simple clipping: stop if x off-screen by more than FONT_WIDTH */
        if (x >= buffer->resolution.width) {
			break;
		}
    }
}

void video_draw_line(struct video_buffer *buffer,
					 int x0, int y0, int x1, int y1,
					 uint32_t color) {
    /* Clip trivially: skip if completely outside (optional, minimal check). */
    if (
		(x0 < 0 && x1 < 0) ||
		(y0 < 0 && y1 < 0) ||
        (x0 >= buffer->resolution.width && x1 >= buffer->resolution.width) ||
        (y0 >= buffer->resolution.height && y1 >= buffer->resolution.height)) {
        return;
	}

	int pitch = buffer->resolution.width * buffer->resolution.bpp / 8;
	int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;  /* error term */

    while (1) {
        /* Draw pixel if inside bounds */
        if (x0 < buffer->resolution.width && y0 < buffer->resolution.height) {
            video_set_pixel(buffer->memory, pitch,
							buffer->resolution.bpp / 8,
							x0, y0,
							color);
		}

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
        if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
    }
}

void video_draw_rect(struct video_buffer *buffer,
					 int x, int y, int width, int height,
					 uint32_t color)
{
    int x2 = x + width - 1;
    int y2 = y + height - 1;

    if (width <= 0 || height <= 0) return;

    /* Top */
    video_draw_line(buffer, x, y, x2, y, color);
    /* Bottom */
    video_draw_line(buffer, x, y2, x2, y2, color);
    /* Left */
    video_draw_line(buffer, x, y, x, y2, color);
    /* Right */
    video_draw_line(buffer, x2, y, x2, y2, color);
}

void video_draw_rect_filled(
	struct video_buffer *buffer,
	int x, int y, int width, int height,
	uint32_t color)
{
    if (width <= 0 || height <= 0) return;

    int x_end = x + width;
    int y_end = y + height;

    /* Clip against screen bounds */
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x_end > buffer->resolution.width) x_end = buffer->resolution.width;
    if (y_end > buffer->resolution.height) y_end = buffer->resolution.height;

	int pitch = buffer->resolution.width * buffer->resolution.bpp / 8;
	int bytes_per_pixel = buffer->resolution.bpp / 8;

	for (int py = y; py < y_end; ++py) {
        for (int px = x; px < x_end; ++px) {
            video_set_pixel(buffer->memory, pitch, bytes_per_pixel, px, py, color);
        }
    }
}

struct video_buffer _video_buffer;

struct video_buffer *video_buffer_allocate(struct video_device *device,
										   struct video_resolution *resolution) {
	/* TODO: Create real allocation */
	/* TODO: map pages */
	_video_buffer.memory = (uint8_t*)0x20000;
	mem_copy(&_video_buffer.resolution, resolution, sizeof(struct video_resolution));
	return &_video_buffer;
}

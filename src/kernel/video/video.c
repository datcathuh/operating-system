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

void video_draw_string(struct video_device *device,
					   const uint8_t *font,
					   int px, int py,
					   const char *s,
					   uint32_t fg_color,
					   uint32_t bg_color,
					   int scale) {
	int pitch = device->resolution->width * device->resolution->bpp / 8;
    int x = px;
    int y = py;
    while (*s) {
        if (*s == '\n') {
            x = px; /* return to start */
            y += FONT_HEIGHT * scale;
            s++;
            continue;
        }
        video_draw_char(
			device->vidmem,
			pitch,
			device->resolution->bpp / 8,
			device->resolution->width,
			device->resolution->height,
			font, (unsigned char)*s,
			x,
			y,
			fg_color,
			bg_color,
			scale);
        x += FONT_WIDTH * scale;
        s++;
        /* simple clipping: stop if x off-screen by more than FONT_WIDTH */
        if (x >= device->resolution->width) {
			break;
		}
    }
}

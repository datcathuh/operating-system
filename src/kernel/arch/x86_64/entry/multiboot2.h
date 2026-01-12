#pragma once

#include "types.h"

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36D76289
#define MULTIBOOT2_TAG_TYPE_END 0
#define MULTIBOOT2_TAG_TYPE_MMAP 6
#define MULTIBOOT2_TAG_TYPE_FRAMEBUFFER 8
#define MULTIBOOT2_TAG_TYPE_ACPI_1 14
#define MULTIBOOT2_TAG_TYPE_ACPI_2 15

#define MULTIBOOT2_MEMORY_AVAILABLE 1

struct multiboot2_tag {
	uint32_t type;
	uint32_t size;
};

struct multiboot2_mmap_entry {
	uint64_t addr;
	uint64_t len;
	uint32_t type;
	uint32_t zero;
};

struct multiboot2_tag_mmap {
	uint32_t type;
	uint32_t size;
	uint32_t entry_size;
	uint32_t entry_version;
	struct multiboot2_mmap_entry entries[];
};

struct multiboot2_tag_framebuffer {
	uint32_t type;
	uint32_t size;
	uint64_t addr;
	uint32_t pitch;
	uint32_t width;
	uint32_t height;
	uint8_t bpp;
	uint8_t fbtype;
	uint16_t reserved;
};

struct multiboot2_tag_framebuffer_rgb {
	uint8_t red_field_position;
	uint8_t red_mask_size;
	uint8_t green_field_position;
	uint8_t green_mask_size;
	uint8_t blue_field_position;
	uint8_t blue_mask_size;
};

struct multiboot2_tag_acpi {
	uint32_t type;
	uint32_t size;
	uint8_t rsdp[];
};

void multiboot2_parse(void *mb_addr);
struct multiboot2_tag_framebuffer *multiboot2_get_framebuffer(void);
void *multiboot2_get_acpi_rsdp(void);

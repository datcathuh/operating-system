#include "memory.h"
#include "multiboot2.h"

static struct multiboot2_tag_framebuffer fb = {0};
static struct multiboot2_tag_acpi *multiboot2_tag_acpi = 0;

static void multiboot2_parse_tag_framebuffer(struct multiboot2_tag *tag) {
	mem_copy(&fb, tag, sizeof(struct multiboot2_tag_framebuffer));
}

struct multiboot2_tag_framebuffer *multiboot2_get_framebuffer(void) {
	if (fb.type == MULTIBOOT2_TAG_TYPE_FRAMEBUFFER) {
		return &fb;
	}
	return NULL;
}

void multiboot2_parse(void *mb_addr) {
	uint8_t *base = (uint8_t *)mb_addr;

	uint32_t total_size = *(uint32_t *)base;
	(void)total_size; // optional sanity check

	struct multiboot2_tag *tag = (struct multiboot2_tag *)(base + 8);

	while (tag->type != MULTIBOOT2_TAG_TYPE_END) {
		switch (tag->type) {
		case MULTIBOOT2_TAG_TYPE_FRAMEBUFFER:
			multiboot2_parse_tag_framebuffer(tag);
			break;
		case MULTIBOOT2_TAG_TYPE_ACPI_1:
		case MULTIBOOT2_TAG_TYPE_ACPI_2:
			multiboot2_tag_acpi = (struct multiboot2_tag_acpi *)tag;
			break;
		default:
			/* ignore */
			break;
		}

		/* tags are 8-byte aligned */
		tag =
			(struct multiboot2_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
	}
}

void *multiboot2_get_acpi_rsdp(void) {
	if (multiboot2_tag_acpi) {
		return (void *)multiboot2_tag_acpi->rsdp;
	}
	return NULL;
}

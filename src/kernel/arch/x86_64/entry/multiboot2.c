#include "memory.h"
#include "multiboot2.h"
#include "serial.h"

static struct multiboot2_tag_framebuffer fb = {0};
static struct multiboot2_tag_acpi *multiboot2_tag_acpi = 0;

static void multiboot2_parse_tag_framebuffer(struct multiboot2_tag *tag) {
	mem_copy(&fb, tag, sizeof(struct multiboot2_tag_framebuffer));
}

static void multiboot2_parse_tag_mmap(struct multiboot2_tag *tag) {
	struct multiboot2_tag_mmap *mmap = (struct multiboot2_tag_mmap *)tag;

	uint64_t entries = (mmap->size - sizeof(*mmap)) / mmap->entry_size;

	uint64_t total_memory = 0;
	serial_puts("Memory: \n");
	for (uint64_t i = 0; i < entries; i++) {
		struct multiboot2_mmap_entry *e =
			(struct multiboot2_mmap_entry *)((uint8_t *)mmap->entries +
		                                     i * mmap->entry_size);

		if (e->type == MULTIBOOT2_MEMORY_AVAILABLE) {
			total_memory += e->len;

			serial_puts("  ");
			serial_put_hex64(e->addr);
			serial_puts(": ");
			serial_put_dec(e->len);
			serial_puts(" bytes\n");

			/* TODO Add memory add_usable_memory(e->addr, e->len); */
		}
	}
	serial_puts(" bytes\n");
	serial_puts("  Total memory: ");
	serial_put_dec(total_memory);
	serial_puts(" bytes\n");
	serial_puts("\n");
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
		case MULTIBOOT2_TAG_TYPE_MMAP:
			multiboot2_parse_tag_mmap(tag);
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

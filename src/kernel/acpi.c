#include "acpi.h"
#include "io.h"
#include "memory.h"
#include "serial.h"

static struct acpi_lapic acpi_lapics[ACPI_LAPIC_LIMIT];
static int acpi_lapics_count = 0;

struct acpi_ioapic acpi_ioapics[ACPI_IOAPIC_LIMIT];
static int acpi_ioapics_count = 0;

static void serial_puthex32(uint32_t v) {
	char buf[11];
	buf[10] = 0;
	for (int i = 9; i >= 0; --i) {
		uint8_t nib = v & 0xF;
		buf[i] = (nib < 10) ? ('0' + nib) : ('A' + nib - 10);
		v >>= 4;
	}
	serial_puts("0x");
	serial_puts(buf);
}

static void serial_puthex8(uint8_t v) {
	char buf[3];
	buf[2] = 0;
	buf[1] = (v & 0xF) < 10 ? ('0' + (v & 0xF)) : ('A' + (v & 0xF) - 10);
	v >>= 4;
	buf[0] = (v & 0xF) < 10 ? ('0' + (v & 0xF)) : ('A' + (v & 0xF) - 10);
	serial_puts("0x");
	serial_puts(buf);
}

/* --- ACPI structures (packed) --- */

#define ACPI_SIG_RSDP "RSD PTR "

typedef struct __attribute__((packed)) {
	char signature[8]; // "RSD PTR "
	uint8_t checksum;  // sum of first 20 bytes == 0
	char oemid[6];
	uint8_t revision;      // 0 for v1.0, 2 for v2+
	uint32_t rsdt_address; // physical address of RSDT
	/* v2+ fields */
	uint32_t length;       // total length of RSDP (36)
	uint64_t xsdt_address; // 64-bit physical address of XSDT
	uint8_t extended_checksum;
	uint8_t reserved[3];
} rsdp_descriptor_t;

typedef struct __attribute__((packed)) {
	char signature[4]; // e.g., "RSDT", "XSDT", "APIC"
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemid[6];
	char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} acpi_sdt_header_t;

/* MADT (APIC) specific header: 32-bit Local APIC address + flags */
typedef struct __attribute__((packed)) {
	acpi_sdt_header_t header;
	uint32_t local_apic_address;
	uint32_t flags;
	/* followed by variable-length entries */
} acpi_madt_t;

/* MADT entry header */
typedef struct __attribute__((packed)) {
	uint8_t type;
	uint8_t length;
} madt_entry_header_t;

/* Type 0: Processor Local APIC */
typedef struct __attribute__((packed)) {
	madt_entry_header_t hdr;
	uint8_t acpi_processor_id;
	uint8_t apic_id;
	uint32_t flags; /* bit0 = enabled */
} madt_local_apic_t;

/* Type 1: IO APIC */
typedef struct __attribute__((packed)) {
	madt_entry_header_t hdr;
	uint8_t ioapic_id;
	uint8_t reserved;
	uint32_t ioapic_address;
	uint32_t gsi_base;
} madt_ioapic_t;

/* --- Helpers --- */

static uint8_t acpi_checksum(const uint8_t *addr, int len) {
	uint8_t sum = 0;
	for (int i = 0; i < len; ++i)
		sum = (uint8_t)(sum + addr[i]);
	return sum;
}

static int rsdp_validate(rsdp_descriptor_t *rsdp) {
	if (rsdp == NULL)
		return 0;
	for (int i = 0; i < 8; ++i)
		if (rsdp->signature[i] != ACPI_SIG_RSDP[i])
			return 0;
	if (rsdp->revision == 0) {
		return acpi_checksum((uint8_t *)rsdp, 20) == 0;
	} else {
		if (rsdp->length < 36)
			return 0;
		return acpi_checksum((uint8_t *)rsdp, rsdp->length) == 0;
	}
}

static rsdp_descriptor_t *find_rsdp(void) {
	uintptr_t start = 0x000E0000U;
	uintptr_t end = 0x00100000U;
	for (uintptr_t p = start; p < end; p += 16) {
		rsdp_descriptor_t *candidate = (rsdp_descriptor_t *)p;
		if (candidate->signature[0] == 'R' && candidate->signature[1] == 'S' &&
		    candidate->signature[2] == 'D' && candidate->signature[3] == ' ') {
			if (rsdp_validate(candidate))
				return candidate;
		}
	}
	return NULL;
}

static void print_sig4(const char *sig) {
	for (int i = 0; i < 4; ++i) {
		char c = sig[i];
		if (c >= 32 && c < 127)
			serial_putc(c);
		else
			serial_putc('?');
	}
}

/* Find an SDT by signature by scanning RSDT or XSDT entries */
static uintptr_t acpi_find_table_from_rsdt(acpi_sdt_header_t *rsdt,
                                           const char *sig) {
	if (!rsdt)
		return 0;
	if (acpi_checksum((uint8_t *)rsdt, rsdt->length) != 0)
		return 0;
	uint32_t entries = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
	uint32_t *list = (uint32_t *)((uint8_t *)rsdt + sizeof(acpi_sdt_header_t));
	for (uint32_t i = 0; i < entries; ++i) {
		acpi_sdt_header_t *hdr = (acpi_sdt_header_t *)(uintptr_t)list[i];
		if (hdr->signature[0] == sig[0] && hdr->signature[1] == sig[1] &&
		    hdr->signature[2] == sig[2] && hdr->signature[3] == sig[3])
			return (uintptr_t)hdr;
	}
	return 0;
}

static uintptr_t acpi_find_table_from_xsdt(acpi_sdt_header_t *xsdt,
                                           const char *sig) {
	if (!xsdt)
		return 0;
	if (acpi_checksum((uint8_t *)xsdt, xsdt->length) != 0)
		return 0;
	uint32_t entries = (xsdt->length - sizeof(acpi_sdt_header_t)) / 8;
	uint64_t *list = (uint64_t *)((uint8_t *)xsdt + sizeof(acpi_sdt_header_t));
	for (uint32_t i = 0; i < entries; ++i) {
		acpi_sdt_header_t *hdr = (acpi_sdt_header_t *)(uintptr_t)list[i];
		if (hdr->signature[0] == sig[0] && hdr->signature[1] == sig[1] &&
		    hdr->signature[2] == sig[2] && hdr->signature[3] == sig[3])
			return (uintptr_t)hdr;
	}
	return 0;
}

/* Parse generic SDT table and print header info */
static void parse_sdt_table(uintptr_t phys_addr) {
	acpi_sdt_header_t *hdr = (acpi_sdt_header_t *)phys_addr;
	serial_puts("    ");
	print_sig4(hdr->signature);
	serial_puts(" @ ");
	serial_puthex32((uint32_t)phys_addr);
	serial_puts(" len=");
	serial_puthex32(hdr->length);
	serial_puts("\n");
}

static void parse_rsdt(uintptr_t rsdt_phys) {
	/* RSDT – Root System Description Table */
	acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *)rsdt_phys;
	if (acpi_checksum((uint8_t *)rsdt, rsdt->length) != 0) {
		serial_puts("  RSDT checksum invalid\n");
		return;
	}

	uint64_t page_base = rsdt_phys & ~0xFFFULL;
	uint64_t offset = rsdt_phys & 0xFFFULL;
	uint64_t total_size = offset + rsdt->length;
	uint64_t page_count = (total_size + 0xFFF) >> 12;
	mem_page_map_n(page_base, page_base, page_count, MEM_PAGE_PRESENT | MEM_PAGE_NX);

	uint32_t entries = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
	uint32_t *list = (uint32_t *)((uint8_t *)rsdt + sizeof(acpi_sdt_header_t));
	serial_puts("  RSDT entries: ");
	serial_puthex32(entries);
	serial_puts("\n");
	for (uint32_t i = 0; i < entries; ++i) {
		parse_sdt_table((uintptr_t)list[i]);
	}
}

static void parse_xsdt(uintptr_t xsdt_phys) {
	/* XSDT – Extended System Description Table */
	acpi_sdt_header_t *xsdt = (acpi_sdt_header_t *)xsdt_phys;
	if (acpi_checksum((uint8_t *)xsdt, xsdt->length) != 0) {
		serial_puts("XSDT checksum invalid\n");
		return;
	}
	uint32_t entries = (xsdt->length - sizeof(acpi_sdt_header_t)) / 8;
	uint64_t *list = (uint64_t *)((uint8_t *)xsdt + sizeof(acpi_sdt_header_t));
	serial_puts("  XSDT entries: ");
	serial_puthex32(entries);
	serial_puts("\n");
	for (uint32_t i = 0; i < entries; ++i) {
		parse_sdt_table((uintptr_t)list[i]);
	}
}

static void parse_madt(uintptr_t madt_phys) {
	/* MADT – Multiple APIC Description Table */
	acpi_madt_t *madt = (acpi_madt_t *)madt_phys;
	if (acpi_checksum((uint8_t *)madt, madt->header.length) != 0) {
		serial_puts("MADT checksum invalid\n");
		return;
	}
	serial_puts("  MADT @ ");
	serial_puthex32((uint32_t)madt_phys);
	serial_puts(" len=");
	serial_puthex32(madt->header.length);
	serial_puts(" local_apic=");
	serial_puthex32(madt->local_apic_address);
	serial_puts(" flags=");
	serial_puthex32(madt->flags);
	serial_puts("\n");

	uint8_t *ent = (uint8_t *)madt + sizeof(acpi_madt_t);
	uint8_t *end = (uint8_t *)madt + madt->header.length;

	serial_puts("  MADT entries:\n");
	while (ent + sizeof(madt_entry_header_t) <= end) {
		madt_entry_header_t *h = (madt_entry_header_t *)ent;
		if (h->length == 0)
			break; // avoid infinite loop
		switch (h->type) {
		case 0: {
			if (ent + sizeof(madt_local_apic_t) <= end) {
				madt_local_apic_t *la = (madt_local_apic_t *)ent;
				serial_puts("    Processor Local APIC: ACPI CPU ID=");
				serial_puthex8(la->acpi_processor_id);
				serial_puts(" APIC ID=");
				serial_puthex8(la->apic_id);
				serial_puts(" flags=");
				serial_puthex32(la->flags);
				if (la->flags & 1)
					serial_puts(" (ENABLED)");
				else
					serial_puts(" (DISABLED)");
				serial_puts("\n");

				acpi_lapics[acpi_lapics_count].cpuid = la->acpi_processor_id;
				acpi_lapics[acpi_lapics_count].lapicid = la->apic_id;
				acpi_lapics[acpi_lapics_count].flags = la->flags;
				acpi_lapics_count++;
			}
			break;
		}
		case 1: {
			if (ent + sizeof(madt_ioapic_t) <= end) {
				madt_ioapic_t *io = (madt_ioapic_t *)ent;
				serial_puts("    IO APIC: ID=");
				serial_puthex8(io->ioapic_id);
				serial_puts(" addr=");
				serial_puthex32(io->ioapic_address);
				serial_puts(" gsi_base=");
				serial_puthex32(io->gsi_base);
				serial_puts("\n");

				acpi_ioapics[acpi_ioapics_count].ioapicid = io->ioapic_id;
				acpi_ioapics[acpi_ioapics_count].address = io->ioapic_address;
				acpi_ioapics[acpi_ioapics_count].gsi_base = io->gsi_base;
				acpi_ioapics_count++;
			}
			break;
		}
		default: {
			serial_puts("    type=");
			serial_puthex8(h->type);
			serial_puts(" len=");
			serial_puthex8(h->length);
			serial_puts("\n");
			break;
		}
		}
		ent += h->length;
	}
}

void acpi_parse(void) {
	serial_puts("ACPI:\n");
	mem_page_map_n(0xe0000, 0xe0000, 32, MEM_PAGE_PRESENT | MEM_PAGE_NX);
	rsdp_descriptor_t *rsdp = find_rsdp();
	if (!rsdp) {
		serial_puts("RSDP not found in 0xE0000-0xFFFFF\n");
		return;
	}
	serial_puts("  RSDP @ ");
	serial_puthex32((uint32_t)(uintptr_t)rsdp);
	serial_puts(" revision=");
	serial_puthex32(rsdp->revision);
	serial_puts("\n");

	acpi_sdt_header_t *root = NULL;
	int use_xsdt = 0;

	if (rsdp->revision == 0) {
	    uint64_t page_base = rsdp->rsdt_address & ~0xFFFULL;
		mem_page_map_n(page_base, page_base, 1, MEM_PAGE_PRESENT | MEM_PAGE_NX);

		parse_rsdt(rsdp->rsdt_address);
		root = (acpi_sdt_header_t *)(uintptr_t)rsdp->rsdt_address;
		use_xsdt = 0;
	} else {
		if (rsdp->xsdt_address != 0) {
			serial_puts("  XSDT @ ");
			serial_puthex32((uint32_t)rsdp->xsdt_address);
			serial_puts("\n");
			parse_xsdt(rsdp->xsdt_address);
			root = (acpi_sdt_header_t *)(uintptr_t)rsdp->xsdt_address;
			use_xsdt = 1;
		} else {
			serial_puts("  Fallback to RSDT @ ");
			serial_puthex32(rsdp->rsdt_address);
			serial_puts("\n");
			parse_rsdt(rsdp->rsdt_address);
			root = (acpi_sdt_header_t *)(uintptr_t)rsdp->rsdt_address;
			use_xsdt = 0;
		}
	}

	/* Find MADT (signature "APIC") */
	uintptr_t madt_phys = 0;
	if (use_xsdt)
		madt_phys = acpi_find_table_from_xsdt(root, "APIC");
	else
		madt_phys = acpi_find_table_from_rsdt(root, "APIC");

	if (madt_phys) {
		parse_madt(madt_phys);
	} else {
		serial_puts("  MADT (APIC) not found\n");
	}
	serial_puts("\n");
}

void acpi_lapic_get(struct acpi_lapic **lapics, int *count) {
	*lapics = acpi_lapics;
	*count = acpi_lapics_count;
}

void acpi_ioapic_get(struct acpi_ioapic **ioapics, int *count) {
	*ioapics = acpi_ioapics;
	*count = acpi_ioapics_count;
}


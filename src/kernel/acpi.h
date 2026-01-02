#pragma once

#include "types.h"

#define ACPI_LAPIC_LIMIT 32
#define ACPI_IOAPIC_LIMIT 2

struct acpi_lapic {
	uint8_t cpuid;
	uint8_t lapicid;
	uint32_t flags;
};

struct acpi_ioapic {
	uint8_t ioapicid;
	uint32_t address;
	uint32_t gsi_base;
};

uintptr_t acpi_legacy_find_address(void);
void acpi_parse(void* rsdp_address);

void acpi_lapic_get(struct acpi_lapic **lapics, int *count);
void acpi_ioapic_get(struct acpi_ioapic **ioapics, int *count);

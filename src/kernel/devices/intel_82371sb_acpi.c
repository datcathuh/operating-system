#include "intel_82371sb_acpi.h"
#include "string.h"

struct pci_identification intel_82371sb_acpi_identification = {
	.vendor = 0x8086,
	.device = 0x7113
};

static void intel_82371sb_acpi_driver_description(struct pci_device_driver */*driver*/,
												  struct pci_device */*device*/,
												  char *buffer, uint16_t size) {
	str_copy(buffer, size, "82371AB/EB/MB PIIX4 ACPI");
}

static bool intel_82371sb_acpi_driver_initialize(struct pci_device_driver * /*driver*/,
												 struct pci_device * /*device*/) {
	return true;
}

static bool intel_82371sb_acpi_driver_unload(struct pci_device_driver * /*driver*/,
											 struct pci_device * /*device*/) {
	return true;
}

struct pci_device_driver intel_82371sb_acpi_driver = {
    .description = intel_82371sb_acpi_driver_description,
    .initialize = intel_82371sb_acpi_driver_initialize,
    .unload = intel_82371sb_acpi_driver_unload
};


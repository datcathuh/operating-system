#include "intel_82371sb.h"
#include "string.h"

struct pci_identification intel_82371sb_identification = {
	.vendor = 0x8086,
	.device = 0x7000
};

static void intel_82371sb_driver_description(struct pci_device_driver */*driver*/,
                                           struct pci_device */*device*/,
                                           char *buffer, uint16_t size) {
	str_copy(buffer, size, "82371SB PIIX3 ISA [Natoma/Triton II]");
}

static bool intel_82371sb_driver_initialize(struct pci_device_driver * /*driver*/,
                                          struct pci_device * /*device*/) {
	return true;
}

static bool intel_82371sb_driver_unload(struct pci_device_driver * /*driver*/,
                                      struct pci_device * /*device*/) {
	return true;
}

struct pci_device_driver intel_82371sb_driver = {
    .description = intel_82371sb_driver_description,
    .initialize = intel_82371sb_driver_initialize,
    .unload = intel_82371sb_driver_unload
};


#include "intel_440fx.h"
#include "string.h"

struct pci_identification intel_440fx_identification = {
	.vendor = 0x8086,
	.device = 0x1237
};

static void intel_440fx_driver_description(struct pci_device_driver */*driver*/,
                                           struct pci_device */*device*/,
                                           char *buffer, uint16_t size) {
	str_copy(buffer, size, "Intel 440FX chipset");
}

static bool intel_440fx_driver_initialize(struct pci_device_driver * /*driver*/,
                                          struct pci_device * /*device*/) {
	return true;
}

static bool intel_440fx_driver_unload(struct pci_device_driver * /*driver*/,
                                      struct pci_device * /*device*/) {
	return true;
}

struct pci_device_driver intel_440fx_driver = {
    .description = intel_440fx_driver_description,
    .initialize = intel_440fx_driver_initialize,
    .unload = intel_440fx_driver_unload
};


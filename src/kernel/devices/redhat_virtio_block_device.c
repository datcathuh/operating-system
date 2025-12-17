#include "intel_440fx.h"
#include "string.h"

struct pci_identification redhat_virtio_block_device_identification = {
	.vendor = 0x1af4, .device = 0x1001};

static void redhat_virtio_block_device_driver_description(
	struct pci_device_driver * /*driver*/, struct pci_device * /*device*/,
	char *buffer, uint16_t size) {
	str_copy(buffer, size, "Virtio block device");
}

static bool redhat_virtio_block_device_driver_initialize(
	struct pci_device_driver * /*driver*/, struct pci_device * /*device*/) {
	return true;
}

static bool
redhat_virtio_block_device_driver_unload(struct pci_device_driver * /*driver*/,
                                         struct pci_device * /*device*/) {
	return true;
}

struct pci_device_driver redhat_virtio_block_device_driver = {
	.description = redhat_virtio_block_device_driver_description,
	.initialize = redhat_virtio_block_device_driver_initialize,
	.unload = redhat_virtio_block_device_driver_unload};

#include "bga.h"
#include "io.h"
#include "memory.h"
#include "string.h"
#include "video.h"

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID       0x0
#define VBE_DISPI_INDEX_XRES     0x1
#define VBE_DISPI_INDEX_YRES     0x2
#define VBE_DISPI_INDEX_BPP      0x3
#define VBE_DISPI_INDEX_ENABLE   0x4

#define VBE_DISPI_DISABLED       0x00
#define VBE_DISPI_ENABLED        0x01
#define VBE_DISPI_LFB_ENABLED    0x40

#define VBE_DISPI_ID5            0xB0C5

struct video_resolution _video_resolution;
struct pci_identification bga_identification = {
	.vendor = 0x1234,
	.device = 0x1111
};

static void bga_pci_driver_description(struct pci_device_driver *driver,
									   struct pci_device *device,
									   char *buffer,
									   uint16_t size);
static bool bga_pci_driver_initialize(struct pci_device_driver *driver,
                                      struct pci_device *device);
static bool bga_pci_driver_unload(struct pci_device_driver *driver,
								  struct pci_device *device);

struct pci_device_driver bga_driver = {
    .description = bga_pci_driver_description,
    .initialize = bga_pci_driver_initialize,
    .unload = bga_pci_driver_unload
};

void bga_pci_driver_description(struct pci_device_driver */*driver*/,
								struct pci_device */*device*/,
								char *buffer,
								uint16_t size) {
	str_copy(buffer, size, "Bochs VGA");
}

bool bga_pci_driver_initialize(struct pci_device_driver */*driver*/,
                               struct pci_device */*device*/) {
	return true;
}

bool bga_pci_driver_unload(struct pci_device_driver */*driver*/,
                           struct pci_device */*device*/) {
	return false;
}

static inline void bga_write(uint16_t index, uint16_t value) {
    io_outw(VBE_DISPI_IOPORT_INDEX, index);
    io_outw(VBE_DISPI_IOPORT_DATA, value);
}
static inline uint16_t bga_read(uint16_t index) {
    io_outw(VBE_DISPI_IOPORT_INDEX, index);
    return io_inw(VBE_DISPI_IOPORT_DATA);
}

static bool bga_is_available(void) {
    uint16_t id = bga_read(VBE_DISPI_INDEX_ID);
    return (id >= 0xB0C0 && id <= 0xB0C5);
}

static bool bga_device_initialize(struct video_device*) {
	return bga_is_available();
}

static bool bga_device_resolution_set(struct video_device* device, struct video_resolution *res) {
	mem_copy(&_video_resolution, res, sizeof(struct video_resolution));
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write(VBE_DISPI_INDEX_XRES, res->width);
    bga_write(VBE_DISPI_INDEX_YRES, res->height);
    bga_write(VBE_DISPI_INDEX_BPP, res->bpp);
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
	device->resolution = &_video_resolution;
	/* TODO. Read this from PCI bar. */
	device->vidmem = (uint8_t*)0xfd000000;
	return true;
}

static bool bga_device_cleanup(struct video_device*) {
	// Step 1: Disable Bochs/VBE extension
	bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

	// Step 2: Reinitialize VGA miscellaneous output register
	io_outb(0x3C2, 0x63); // 25 MHz, enable color VGA, etc.
	return true;
}

struct video_device _bga_device = {
	.resolution = 0,
	.initialize = bga_device_initialize,
	.resolution_set = bga_device_resolution_set,
	.cleanup = bga_device_cleanup
};

struct video_device *bga_device() {
	return &_bga_device;
}

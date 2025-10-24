#include "types.h"
#include "devices/intel_440fx.h"
#include "io.h"
#include "memory.h"
#include "pci.h"
#include "serial.h"
#include "string.h"
#include "video/bga.h"

static struct pci_device _devices[256] = {0};
static uint16_t _device_count = 0;

static uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (1U << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)slot << 11)
                     | ((uint32_t)func << 8)
                     | (offset & 0xFC);
    io_outl(0xCF8, address);
    return io_inl(0xCFC);
}

void pci_enumerate(pci_enumerate_cb cb) {
	struct pci_device dev;
    for (dev.bus = 0; dev.bus < 256; dev.bus++) {
        for (dev.slot = 0; dev.slot < 32; dev.slot++) {
            for (dev.func = 0; dev.func < 8; dev.func++) {

                uint32_t vendor_device = pci_config_read32(dev.bus, dev.slot, dev.func, 0x00);
                dev.identification.vendor = vendor_device & 0xFFFF;
                if (dev.identification.vendor == 0xFFFF) continue; // no device

                dev.identification.device = (vendor_device >> 16) & 0xFFFF;
                uint32_t classcode = pci_config_read32(dev.bus, dev.slot, dev.func, 0x08);
                dev.class_id = (classcode >> 24) & 0xFF;
                dev.subclass = (classcode >> 16) & 0xFF;

                // Read BARs (6 possible)
                for (int bar = 0; bar < 6; bar++) {
                    dev.bar[bar] = pci_config_read32(dev.bus, dev.slot, dev.func, 0x10 + bar * 4);
                }

				cb(&dev);
            }
        }
    }
}


static void pci_device_create_cb(struct pci_device *device) {
	mem_copy(&_devices[_device_count], device, sizeof(struct pci_device));
	if (device->identification.vendor == bga_identification.vendor &&
		device->identification.device == bga_identification.device) {
		_devices[_device_count].driver = &bga_driver;
	} else if(device->identification.vendor == intel_440fx_identification.vendor &&
			  device->identification.device == intel_440fx_identification.device) {
		_devices[_device_count].driver = &intel_440fx_driver;
	}
	_device_count++;
}

void pci_build_device_tree(void) {
	pci_enumerate(pci_device_create_cb);
}

void pci_device_tree(struct pci_device **devices, uint16_t *device_count) {
	(*devices) = _devices;
	(*device_count) = _device_count;
}

void pci_debug_dump(void) {
	for(int i=0; i < _device_count; i++) {
		struct pci_device *dev = &_devices[i];
		char vendorid[10];
		char devid[10];
		str_hex_from_uint32(vendorid, 10, dev->identification.vendor);
		str_hex_from_uint32(devid, 10, dev->identification.device);
		serial_puts("PCI device found: ");
		serial_puts(vendorid);
		serial_puts(":");
		serial_puts(devid);

		char description[100];
		if(dev->driver) {
			serial_puts(" ");
			dev->driver->description(dev->driver, dev, description, 100);
			serial_puts(description);
		}

		serial_puts("\n");
	}
}

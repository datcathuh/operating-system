#include "types.h"
#include "devices/intel_440fx.h"
#include "devices/intel_82371sb_acpi.h"
#include "devices/intel_82371sb_isa.h"
#include "devices/intel_82371sb_ide.h"
#include "devices/intel_82540em.h"
#include "devices/usb_xhci.h"
#include "io.h"
#include "memory.h"
#include "pci.h"
#include "serial.h"
#include "string.h"
#include "video/bga.h"

static struct pci_device _devices[256] = {0};
static uint16_t _device_count = 0;

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static inline uint32_t pci_config_address(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    return (uint32_t)(
        (1U << 31)               |  /* enable bit */
        ((uint32_t)bus    << 16) |
        ((uint32_t)slot   << 11) |
        ((uint32_t)func   << 8)  |
        (offset & 0xFC)             /* align to dword */
    );
}

uint8_t pci_cfg_read8(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t value = pci_config_read32(bus, slot, func, offset);
	return (uint8_t)((value >> ((offset & 3) * 8)) & 0xFF);
}

uint16_t pci_cfg_read16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t value = pci_config_read32(bus, slot, func, offset);
	return (uint16_t)((value >> ((offset & 2) * 8)) & 0xFFFF);
}

uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = pci_config_address(bus, slot, func, offset);
    io_outl(PCI_CONFIG_ADDRESS, address);
    return io_inl(PCI_CONFIG_DATA);
}

void pci_cfg_write32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t val) {
    uint32_t address = pci_config_address(bus, slot, func, offset);
    io_outl(PCI_CONFIG_ADDRESS, address);
    io_outl(PCI_CONFIG_DATA, val);
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
					/* Mask of the lowest four bits since they are flags. */
                    dev.bar[bar] = pci_config_read32(dev.bus, dev.slot, dev.func, 0x10 + bar * 4) & ~0xF;
                }

				dev.driver = NULL;
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
	} else if(device->identification.vendor == intel_82371sb_isa_identification.vendor &&
			  device->identification.device == intel_82371sb_isa_identification.device) {
		_devices[_device_count].driver = &intel_82371sb_isa_driver;
	} else if(device->identification.vendor == intel_82371sb_ide_identification.vendor &&
			  device->identification.device == intel_82371sb_ide_identification.device) {
		_devices[_device_count].driver = &intel_82371sb_ide_driver;
	} else if(device->identification.vendor == intel_82371sb_acpi_identification.vendor &&
			  device->identification.device == intel_82371sb_acpi_identification.device) {
		_devices[_device_count].driver = &intel_82371sb_acpi_driver;
	} else if(device->identification.vendor == intel_82540em_identification.vendor &&
			  device->identification.device == intel_82540em_identification.device) {
		_devices[_device_count].driver = &intel_82540em_driver;
	} else if(device->identification.vendor == usb_xhci_identification.vendor &&
			  device->identification.device == usb_xhci_identification.device) {
		_devices[_device_count].driver = &usb_xhci_driver;
	}
	if(_devices[_device_count].driver) {
		_devices[_device_count].driver->initialize(_devices[_device_count].driver, &_devices[_device_count]);
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
	serial_puts("PCI devices:\n");
	for(int i=0; i < _device_count; i++) {
		struct pci_device *dev = &_devices[i];
		char vendorid[10];
		char devid[10];
		str_hex_from_uint32(vendorid, 10, dev->identification.vendor);
		str_hex_from_uint32(devid, 10, dev->identification.device);
		serial_puts("  ");
		serial_puts(vendorid);
		serial_puts(":");
		serial_puts(devid);

		char description[100];
		if(dev->driver) {
			serial_puts(" ");
			dev->driver->description(dev->driver, dev, description, 100);
			serial_puts(description);
		}

		for(int i=0;i < 6; i++) {
			if(dev->bar[i] == 0) {
				continue;
			}
			serial_puts("\n");
			serial_puts("    bar[");
			char x[10];
			str_hex_from_uint32(x, 10, i);
			serial_puts(x);
			serial_puts("]: ");
			str_hex_from_uint32(x, 10, dev->bar[i]);
			serial_puts(x);
		}

		serial_puts("\n");
	}
	serial_puts("\n");
}

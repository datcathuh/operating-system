#include "types.h"
#include "io.h"
#include "pci.h"

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
                dev.vendor = vendor_device & 0xFFFF;
                if (dev.vendor == 0xFFFF) continue; // no device

                dev.device = (vendor_device >> 16) & 0xFFFF;
                uint32_t classcode = pci_config_read32(dev.bus, dev.slot, dev.func, 0x08);
                dev.class_id = (classcode >> 24) & 0xFF;
                dev.subclass = (classcode >> 16) & 0xFF;

                //printf("Bus %u, Slot %u, Func %u: Vendor %04X Device %04X Class %02X:%02X\n",
                //       bus, slot, func, vendor, device, class_id, subclass);

                // Read BARs (6 possible)
                for (int bar = 0; bar < 6; bar++) {
                    dev.bar[bar] = pci_config_read32(dev.bus, dev.slot, dev.func, 0x10 + bar * 4);
                }

				cb(&dev);
                // Optional: stop if Bochs VGA found
                if (dev.vendor == 0x1234 && dev.device == 0x1111) {
                    // uint32_t bar0 = pci_config_read32(dev.bus, dev.slot, dev.func, 0x10);
                    // uint32_t fb_addr = bar0 & 0xFFFFFFF0;
                    // printf("🟢 Found Bochs/QEMU VGA framebuffer at 0x%08X\n", fb_addr);
                    return;
                }
            }
        }
    }
}

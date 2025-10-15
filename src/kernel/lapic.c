#include "lapic.h"
#include "types.h"

#define LAPIC_BASE_DEFAULT 0xFEE00000
#define LAPIC_REG_ID       0x20
#define LAPIC_REG_EOI      0xB0
#define LAPIC_REG_SVR      0xF0
#define LAPIC_REG_LVT_TIMER 0x320
#define LAPIC_REG_LVT_ERROR 0x370

static volatile uint32_t* lapic = (uint32_t*)LAPIC_BASE_DEFAULT;

static inline void lapic_write(uint32_t reg, uint32_t val) {
    lapic[reg/4] = val;
    (void)lapic[reg/4]; // Read-back to flush write buffer
}

void lapic_default_init(void) {
    // Enable software APIC (bit 8 in SVR)
    uint32_t svr = lapic[LAPIC_REG_SVR/4];
    svr |= 0x100;  // APIC enable
    svr &= ~0xFF;  // Set vector 0 for spurious IRQ (disable actual delivery)
    lapic_write(LAPIC_REG_SVR, svr);

    // Mask all LVT entries (disable timer, error, etc.)
    lapic_write(LAPIC_REG_LVT_TIMER, 1 << 16); // mask bit
    lapic_write(LAPIC_REG_LVT_ERROR, 1 << 16);

    // Send EOI just in case something was pending
    lapic_write(LAPIC_REG_EOI, 0);
}

#include "io.h"
#include "pit.h"

/* ---- PIT-based polling wait (no IRQs): program PIT channel 0 to ~100Hz and
   poll its counter. Explanation:
   - We program PIT channel 0, access mode lobyte/hibyte, mode 2 (rate
   generator)
   - Divisor chosen to produce ~100 Hz (1193182 / 11932 ~= 100)
   - We then repeatedly latch and read the current counter and compute elapsed
   ticks.
*/
void pit_wait_milliseconds(uint32_t milliseconds) {
	const uint32_t PIT_FREQ = 1193182U;
	const uint32_t TARGET_HZ = 1000; /* 1000 ticks per second */
	uint16_t divisor = (uint16_t)(PIT_FREQ / TARGET_HZ);
	if (divisor == 0)
		divisor = 1;

	/* Program PIT: channel 0, lobyte/hibyte, mode 2 (rate generator) */
	io_outb(0x43, 0x34);
	io_outb(0x40, divisor & 0xFF);
	io_outb(0x40, divisor >> 8);

	uint32_t ticks_needed = milliseconds;
	uint32_t ticks = 0;

	/* Read initial counter value */
	io_outb(0x43, 0x00); // latch count
	uint16_t last = io_inb(0x40) | (io_inb(0x40) << 8);

	while (ticks < ticks_needed) {
		io_outb(0x43, 0x00); // latch again
		uint16_t now = io_inb(0x40) | (io_inb(0x40) << 8);

		if (now > last) {
			/* Counter wrapped around once */
			ticks++;
		}
		last = now;
	}
}

void sleep_ms(uint32_t ms) {
	pit_wait_milliseconds(ms);
}

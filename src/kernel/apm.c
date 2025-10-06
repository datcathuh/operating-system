#include "apm.h"
#include "io.h"

/*

  APM = Advanced Power Management

 */

void apm_power_off(void) {
	/* This works only for QEMU */
	io_outw(0x604, 0x2000);
}

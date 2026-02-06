#include "sleep.h"

/* Architecture dependent function. Could also be implemented using
 * the scheduler.
 */
void (*sleep_ms)(uint32_t ms);

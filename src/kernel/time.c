#include "time.h"

void clocksource_register(clocksource_t *cs) {}

bool clocksource_select(const char *name) { return false;}

ns_t clocksource_read_ns(void) { return 0; }

uint64_t clocksource_read_ticks(void) { return 0; }

ns_t clocksource_ticks_to_ns(uint64_t ticks) { return 0; }

void clockevent_register(clockevent_t *ce) {}

int clockevent_set_periodic(uint32_t hz) { return -1; }

int clockevent_set_oneshot(ns_t deadline_ns) { return -1; }

void clockevent_shutdown(void) {}

void clockevent_set_handler(void (*handler)(clockevent_t *ce)) {}

bool clockevent_select(const char *name) { return false; }

ns_t time_ns(void) { return 0; }

uint64_t time_ms(void) { return 0; }

void time_init(void) {}

#pragma once
#include "types.h"

typedef uint64_t ns_t;

#define CLOCK_NAME_LEN 32

typedef struct clocksource {
	char name[CLOCK_NAME_LEN];

	/*
	 * Read the raw hardware counter. Must be safe to call from any context
	 * (IRQ, NMI, early boot). Should not take locks.
	 */
	uint64_t (*read)(struct clocksource *cs);

	/*
	 * Nominal counter frequency in Hz.
	 * Used to convert: ns = (ticks * ns_per_tick_mul) >> ns_per_tick_shift
	 * The driver fills freq_hz; the registration code computes the
	 * mul/shift pair so consumers never do division in the hot path.
	 */
	uint64_t freq_hz;
	uint32_t mul;  /* computed at registration */
	uint8_t shift; /* computed at registration */

	/*
	 * Rating: higher = preferred. Suggested values:
	 *   400  Perfect:   TSC (invariant, synced across CPUs)
	 *   300  Good:      HPET main counter
	 *   200  OK:        ACPI PM timer
	 *    50  Fallback:  PIT-derived (not a real free counter; avoid if
	 * possible)
	 */
	int rating;

	/* Opaque driver private data. */
	void *priv;

	/* Intrusive linked list — owned by the registration layer. */
	struct clocksource *next;
} clocksource_t;

void clocksource_register(clocksource_t *cs);

bool clocksource_select(const char *name);

ns_t clocksource_read_ns(void);

uint64_t clocksource_read_ticks(void);

ns_t clocksource_ticks_to_ns(uint64_t ticks);

/* =========================================================================
 * Clock Event  —  a device that fires an interrupt at a programmed time.
 *
 * Drives the scheduler tick and (later) tickless oneshot wakeups.
 * Drivers fill in a clockevent_t and call clockevent_register().
 * ========================================================================= */

typedef enum {
	CLOCKEVENT_MODE_UNINIT = 0,
	CLOCKEVENT_MODE_SHUTDOWN = 1, /* IRQs disabled, device idle          */
	CLOCKEVENT_MODE_PERIODIC = 2, /* fires every tick_ns nanoseconds     */
	CLOCKEVENT_MODE_ONESHOT = 3,  /* fires once, then enters SHUTDOWN    */
} clockevent_mode_t;

/* Feature flags — advertised by the driver. */
#define CLOCKEVENT_FEAT_PERIODIC (1u << 0) /* supports periodic mode     */
#define CLOCKEVENT_FEAT_ONESHOT (1u << 1)  /* supports oneshot mode      */
#define CLOCKEVENT_FEAT_PER_CPU (1u << 2)  /* one instance per CPU       */

typedef struct clockevent {
	char name[CLOCK_NAME_LEN];
	uint32_t features;

	/*
	 * Hardware limits. The kernel will not request intervals outside these
	 * bounds. Both in nanoseconds.
	 */
	ns_t min_ns;
	ns_t max_ns;

	/*
	 * Rating: higher = preferred. Suggested values:
	 *   400  Local APIC timer  (per-CPU, oneshot+periodic, tickless-ready)
	 *   300  HPET comparator   (global, oneshot+periodic)
	 *   100  PIT               (global, periodic only on most setups)
	 */
	int rating;

	/*
	 * Current programmed mode. Set by the registration layer, not the driver.
	 * Drivers should treat this as read-only and check it defensively.
	 */
	clockevent_mode_t mode;

	/*
	 * Set periodic mode. interval_ns is the requested period.
	 * The driver should round to the nearest hardware-representable value
	 * and store the actual period in tick_ns.
	 * Returns 0 on success, negative on error.
	 */
	int (*set_periodic)(struct clockevent *ce, ns_t interval_ns);

	/*
	 * Set oneshot mode. The device fires exactly once after deadline_ns
	 * nanoseconds from now, then goes idle.
	 * Returns 0 on success, negative on error.
	 */
	int (*set_oneshot)(struct clockevent *ce, ns_t deadline_ns);

	/*
	 * Shut the device down. No more IRQs until set_periodic/set_oneshot
	 * is called again.
	 */
	void (*shutdown)(struct clockevent *ce);

	/*
	 * Called by the IRQ handler each time the device fires.
	 * The registration layer installs this; drivers must call it at the
	 * end of their ISR.
	 *
	 * In ONESHOT mode the device transitions to SHUTDOWN before this fires,
	 * so the handler can immediately re-arm if needed.
	 */
	void (*event_handler)(struct clockevent *ce);

	/* Actual programmed period (filled in by the driver after set_periodic). */
	ns_t tick_ns;

	/* Opaque driver private data. */
	void *priv;

	/* Intrusive linked list — owned by the registration layer. */
	struct clockevent *next;
} clockevent_t;

void clockevent_register(clockevent_t *ce);

/*
 * Program the active clock event into periodic mode at the given rate.
 * hz=10 gives your current 10Hz tick; hz=1000 gives a 1ms tick.
 * Returns 0 on success, negative if no suitable device is registered.
 */
int clockevent_set_periodic(uint32_t hz);

/*
 * Program the active clock event into oneshot mode.
 * Fires once after deadline_ns nanoseconds, then goes idle.
 * Your tickless path will call this instead of set_periodic.
 * Returns 0 on success.
 */
int clockevent_set_oneshot(ns_t deadline_ns);

/*
 * Shut down the active clock event (e.g. during CPU hotplug or suspend).
 */
void clockevent_shutdown(void);

/*
 * Install a custom event handler on the active clock event.
 * This is what your scheduler calls to hook the tick:
 *
 *   clockevent_set_handler(scheduler_tick);
 *
 * The handler receives the clockevent pointer so it can re-arm in
 * oneshot mode without a global lookup.
 */
void clockevent_set_handler(void (*handler)(clockevent_t *ce));

/*
 * Force a specific event device active by name.
 * Returns false if not registered.
 */
bool clockevent_select(const char *name);

/* =========================================================================
 * Wall clock / monotonic time  —  built on top of the clock source.
 *
 * Thin helpers so the rest of the kernel never calls clocksource_read_ns()
 * directly and you have one place to add NTP correction / monotonic
 * guarantees later.
 * ========================================================================= */

/*
 * Nanoseconds since an arbitrary epoch (boot).
 * Monotonic — never goes backwards.
 */
ns_t time_ns(void);

/*
 * Milliseconds since boot. Handy for sleep_ms and debug logging.
 */
uint64_t time_ms(void);

/*
 * Called once at boot after a clock source is registered.
 * Sets the internal epoch so time_ns() starts near zero.
 */
void time_init(void);

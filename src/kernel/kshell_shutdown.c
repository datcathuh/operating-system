#include "apm.h"
#include "kshell.h"
#include "kshell_shutdown.h"

static void kshell_shutdown_cb() {
	apm_power_off();
}

void kshell_shutdown_register() {
	struct kshell_command cmd = {
		.name = "shutdown",
		.callback = kshell_shutdown_cb
	};
	kshell_register_command(&cmd);
}

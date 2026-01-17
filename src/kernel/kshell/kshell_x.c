#include "kshell.h"
#include "kshell_x.h"

static void kshell_x_cb() {
}

void kshell_x_register() {
	struct kshell_command cmd = {.name = "x",
	                             .callback = kshell_x_cb};
	kshell_register_command(&cmd);
}

#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct kshell_command {
	char name[20];
	void (*callback)();
};

void kshell_init();
void kshell();
bool kshell_register_command(const struct kshell_command *cmd);
const struct kshell_command *kshell_commands_get();

#ifdef __cplusplus
}
#endif

#pragma once

struct kshell_command {
	char name[20];
	void (*callback)();
};

void kshell_init();
void kshell();
bool kshell_register_command(const struct kshell_command *cmd);

#include "kshell.h"
#include "kshell_task.hpp"
#include "scheduler.h"
#include "video/video.h"

extern "C" {

static void kshell_task_enumerate_cb(struct task *task) {
	struct terminal *terminal = video_current()->terminal;
	terminal->print(terminal, "  ");
	terminal->print(terminal, task->name);
	switch (task->state) {
	case TASK_RUNNING:
		terminal->print(terminal, ", state: RUNNING");
		break;
	case TASK_READY:
		terminal->print(terminal, ", state: READY");
		break;
	case TASK_BLOCKED:
		terminal->print(terminal, ", state: BLOCKED");
		break;
	case TASK_SLEEPING:
		terminal->print(terminal, ", state: SLEEPING");
		break;
	}
	terminal->print(terminal, "\n");
}

static void kshell_task_list_cb() {
	struct terminal *terminal = video_current()->terminal;
	terminal->print(terminal, "Kernel tasks:\n\n");
	scheduler_task_enumerate(kshell_task_enumerate_cb);
}

void kshell_task_register() {
	struct kshell_command cmd = {.name = "task list",
	                             .callback = kshell_task_list_cb};
	kshell_register_command(&cmd);
}
}

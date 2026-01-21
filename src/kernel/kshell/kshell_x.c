#include "kshell.h"
#include "kshell_x.h"
#include "scheduler.h"
#include "task.h"
#include "video/video.h"

static struct task my_task;

static void my_task_cb(void) {
	for (;;) {
		struct video_device *vd = video_current();
		if (vd && vd->terminal) {
			/* TODO This is dangerous. We have no locking around the vd and
			   the terminal. */
			vd->terminal->pos_set(vd->terminal, 0, 0);
			vd->terminal->print(vd->terminal, "Background thread");
		}

		yield();
	}
}

static void kshell_x_cb() {
	task_create(&my_task, my_task_cb);
	scheduler_task_add(&my_task);
	yield();
}

void kshell_x_register() {
	struct kshell_command cmd = {.name = "x", .callback = kshell_x_cb};
	kshell_register_command(&cmd);
}

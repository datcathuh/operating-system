#include "kshell.h"
#include "kshell_x.h"
#include "scheduler.h"
#include "task.h"
#include "video/video.h"

static struct task my_task = {0};

static void my_task_cb(void) {
	static int count = 0;
	for (;;) {
		count += 1;
		struct video_device *vd = video_current();
		if (vd && vd->terminal) {
			/* TODO This is dangerous. We have no locking around the vd and
			   the terminal. */
			int x, y;
			vd->terminal->pos_get(vd->terminal, &x, &y);
			vd->terminal->pos_set(vd->terminal, 0, 20);
			vd->terminal->print(vd->terminal,
			                    "This is a background thread writing this ");
			vd->terminal->pos_set(vd->terminal, x, y);
		}

		yield();
	}
}

static void kshell_x_cb() {
	if (my_task.name[0] == 0) {
		task_create("display_clock", &my_task, my_task_cb);
		scheduler_task_add(&my_task);
	}
	yield();
}

void kshell_x_register() {
	struct kshell_command cmd = {.name = "x", .callback = kshell_x_cb};
	kshell_register_command(&cmd);
}

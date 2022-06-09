#include "runtime.h"

#include <multitasking/task.h>
#include <runtime/windowmanager.h>

// programs (will be read from hard drive later)
#include <runtime/programs/cube.h>
#include <runtime/programs/editor.h>
#include <runtime/programs/gradient.h>
#include <runtime/programs/tbz.h>

void runtime_start() {
    windowmanager_init();
    task_add(&windowmanager_run, TASK_PRIORITY_VERY_HIGH, 0);

    // program startup
    // task_add(&cube_main, TASK_PRIORITY_MEDIUM, 0);
    task_add(&gradient_main, TASK_PRIORITY_LOW, 0);
    task_add(&tbz_main, TASK_PRIORITY_LOW, 0);
    task_add(&editor_main, TASK_PRIORITY_LOW, 0);
}

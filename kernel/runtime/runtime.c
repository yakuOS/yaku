#include "runtime.h"

#include <multitasking/task.h>
#include <runtime/windowmanager.h>

void runtime_init() {
    task_add(&windowmanager_main, TASK_PRIORITY_VERY_HIGH, 0);
}

#pragma once
#include <drivers/pit.h>
#include <multitasking/task.h>
#include <types.h>

#define MAX_SCHEDULED_TASKS MAX_TASKS * 4
#define SWITCH_INTERVAL_MS 100

extern void switch_to_task(uint64_t* rsp);
void scheduler_schedule_task(task_t* task);
void scheduler_set_task_terminated();
void scheduler_switch_task(uint64_t* rsp);
task_t* scheduler_get_current_task();
void scheduler_sleep(uint32_t ticks);

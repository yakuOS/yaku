#pragma once
#include <drivers/pit.h>
#include <multitasking/task.h>
#include <types.h>

#define MAX_SCHEDULED_TASKS MAX_TASKS * 4
#define SWITCH_INTERVAL_MS 100

extern void switch_to_task(uint64_t* rsp);
static struct task* current_task;
void schedule_tasks();
void scheduler_task();
void schedule_set_task_terminated();
void schedule_init(void* kernel_function);
void schedule_switch(uint64_t* rsp);
struct task* scheduler_get_current_task();
void schedule_sleep(uint32_t ticks);

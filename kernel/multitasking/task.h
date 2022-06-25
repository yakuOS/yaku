#pragma once

#include <multitasking/scheduler.h>
#include <types.h>

#define TASK_STACK_SIZE 1019
#define TASKS_MAX 500

enum task_state {
    TASK_STATE_RUNNING,
    TASK_STATE_SLEEP,
    TASK_STATE_TERMINATED,
    TASK_STATE_WAITING,
    TASK_STATE_PAUSED
};

enum task_priority {
    TASK_PRIORITY_LOW,
    TASK_PRIORITY_MEDIUM,
    TASK_PRIORITY_HIGH,
    TASK_PRIORITY_VERY_HIGH
};

typedef struct task {
    uint64_t* rsp;
    uint64_t stack[TASK_STACK_SIZE];
    uint32_t sleep_till;
    uint32_t pid;
    uint32_t parent_pid;
    enum task_state task_state;
    enum task_priority priority;
    struct task* next;
} task_t;
typedef struct task_parameters {
    uint64_t first; // rdi
    uint64_t second; // rsi
    uint64_t third; // rdx
    uint64_t fourth; // rcx
    uint64_t fifth; // r8
    uint64_t sixth; // r9
} task_parameters_t;

task_t* task_add(void* function, task_parameters_t* parameters, enum task_priority priority, uint32_t parent_pid);
task_t* task_create(void* function, task_parameters_t* parameters);
void task_exit();
void task_terminate(task_t* task, task_t* task_pointing_to);
task_t* task_get_ptr_by_pid(uint32_t pid);
task_t* task_get_ptr_by_parent_pid(uint32_t pid);
void task_kill(uint32_t pid);
void task_sleep(task_t* task, uint32_t ticks);
void task_pause(task_t* task);
void task_resume(task_t* task);

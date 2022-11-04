#include "task.h"

#include <drivers/pit.h>
#include <drivers/serial.h>
#include <interrupts/pic.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <thirdparty/string/string.h>

static uint32_t number_of_tasks = 0;

// create task and schedule it
task_t* task_add(void* function, task_parameters_t* parameters,
                 enum task_priority priority, uint32_t parent_pid) {
    asm("cli");

    if (number_of_tasks == TASKS_MAX) {
        serial_printf("Max tasks reached\n");
        asm("sti");
        return NULL;
    }

    task_t* task = task_create(function, parameters);

    number_of_tasks++;

    task->priority = priority;
    if (scheduler_get_current_task() != NULL) {
        task->parent_pid = scheduler_get_current_task()->pid;
    }
    task->pid = number_of_tasks;
    task->task_state = TASK_STATE_WAITING;
    scheduler_schedule_task(task);

    asm("sti");

    return task;
}

void task_sleep(task_t* task, uint32_t ticks) {
    task->sleep_till = pit_tick_get() + ticks;
    task->task_state = TASK_STATE_SLEEP;
}
void task_change_priority(task_t* task, enum task_priority priority) {
    task->priority = priority;
}
// removes task from schedule-linked-list and frees memory
void task_terminate(task_t* task, task_t* task_pointing_to) {
    pic_mask_irq(0); // dont switch tasks anymore

    if (task == NULL) {
        return;
    }

    if (task_pointing_to->next == task) {
        task_pointing_to->next = task->next;
    } else {
        return;
    }

    free(task); // sizeof(task_t) is 8192 bytes

    number_of_tasks--;

    pic_unmask_irq(0);
}

task_t* task_get_ptr_by_pid(uint32_t pid) {
    task_t* task = scheduler_get_current_task();

    uint32_t counter = 0;
    while (task->pid != pid || counter > number_of_tasks) {
        task = task->next;
        counter++;
    }

    if (task->pid == pid) {
        return task;
    }

    return NULL;
}

task_t* task_get_ptr_by_parent_pid(uint32_t pid) {
    task_t* task = scheduler_get_current_task();

    uint32_t counter = 0;
    while (task->parent_pid != pid && counter > number_of_tasks) {
        task = task->next;
        counter++;
    }

    if (task->parent_pid == pid) {
        return task;
    }

    return NULL;
}

void task_kill(uint32_t pid) {
    if (pid == 0) {
        return;
    }
    asm("cli");

    task_t* task = task_get_ptr_by_pid(pid);

    bool task_found = false;
    task_t* task_pointing_to = task;
    while (!task_found) {
        if (task_pointing_to->next == task) {
            task_found = true;
            task_pointing_to->next = task->next;
        } else {
            task_pointing_to = task_pointing_to->next;
        }
    }

    task_terminate(task, task_pointing_to);

    // terminate tasks where parent_pid == pid
    bool nothing_left = false;
    while (!nothing_left) {
        task = task_get_ptr_by_parent_pid(pid);
        task_kill(task->pid);
        if (task == NULL) {
            nothing_left = true;
        }
    }

    asm("sti");
}

// returned to at the end of a task, sets its state to terminated and waits
void task_exit() {
    scheduler_set_task_terminated();
    for (;;) {
        asm("hlt");
    }
}

void task_pause(task_t* task) {
    task->task_state = TASK_STATE_PAUSED;
}

void task_resume(task_t* task) {
    task->task_state =
        TASK_STATE_SLEEP; // if it did not sleep before pause sleep_till is 0 -> thread
                          // set to task_state_running on next task_switch
}

int32_t task_add_file(task_t* task, file_handle_t* file) {
    for (uint32_t i = 0; i < TASK_FILES_MAX; i++) {
        if (task->files[i] == NULL) {
            task->files[i] = file;
            return (int32_t)i;
        }
    }
    return -1;
}

int task_remove_file(task_t* task, uint64_t file_id) {
    free(task->files[file_id]);
    task->files[file_id] = NULL;
}
static uint64_t rflags;
void set_rflags(uint64_t flags) {
    rflags = flags;
}
// allocates memory for task and sets its stack up
task_t* task_create(void* function, task_parameters_t* parameters) {
    task_t* new_task = (task_t*)malloc(sizeof(task_t)); // sizeof(task_t) = 8192

    memset(new_task, 0, sizeof(task_t));

    // 15 regs for poping in task_switch, 5 for return address
    new_task->rsp = &(new_task->stack[TASK_STACK_SIZE - 21]);

    new_task->stack[TASK_STACK_SIZE - 1] = (uint64_t)&task_exit;
    // new_task->stack[TASK_STACK_SIZE - 2] = 0x00; // return SS
    // get current ss and copy them into new_task->stack[TASK_STACK_SIZE - 2]
    asm("mov %%ss, %0" : "=r"(new_task->stack[TASK_STACK_SIZE - 2]));
    new_task->stack[TASK_STACK_SIZE - 3] =
        (uint64_t) & (new_task->stack[TASK_STACK_SIZE - 1]); // return RSP
    new_task->stack[TASK_STACK_SIZE - 4] = rflags;              // return RFLAGS
    // get current RFLAGS and copy them into new_task->stack[TASK_STACK_SIZE - 4]
    // new_task->stack[TASK_STACK_SIZE - 5] = 0x28;               // return cs
    // mov current cs to new_task->stack[TASK_STACK_SIZE - 5]
    asm("mov %%cs, %0" : "=r"(new_task->stack[TASK_STACK_SIZE - 5]));
    new_task->stack[TASK_STACK_SIZE - 6] = (uint64_t)function; // return address -> rip
    new_task->stack[TASK_STACK_SIZE - 7] =
        (uint64_t) &
        (new_task->stack[TASK_STACK_SIZE - 1]); // rbp popped manually
                                                // TODO: add stack needed for iretq

    if (parameters != NULL) {
        // add parameters to stack so they get popped into the right registers on first
        // task_switch
        new_task->stack[TASK_STACK_SIZE - 13] = parameters->first;
        new_task->stack[TASK_STACK_SIZE - 12] = parameters->second;
        new_task->stack[TASK_STACK_SIZE - 11] = parameters->third;
        new_task->stack[TASK_STACK_SIZE - 10] = parameters->fourth;
        new_task->stack[TASK_STACK_SIZE - 14] = parameters->fifth;
        new_task->stack[TASK_STACK_SIZE - 15] = parameters->sixth;
    }

    return new_task;
}

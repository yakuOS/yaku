#include "task.h"
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <interrupts/pic.h>
#include <multitasking/schedule.h>
#include <thirdparty/string/string.h>

static uint32_t number_of_tasks = 0;

// create task and schedule it
task_t* task_add(void* function, enum task_priority priority, uint32_t parent_pid) {
    asm("cli");
    if (number_of_tasks == MAX_TASKS) {
        serial_printf("Max tasks reached\n");
        asm("sti");
        return;
    }
    task_t* task = task_create(function);
    number_of_tasks++;
    task->priority = priority;
    task->parent_pid = parent_pid;
    task->pid = number_of_tasks;
    task->task_state = TASK_STATE_WAITING;
    schedule_task(task);
    asm("sti");
    return task;
}

void task_sleep(task_t* task, uint32_t ticks) {
    task->sleep_till = pit_tick_get() + ticks;
    task->task_state = TASK_STATE_SLEEP;
}

// removes task from schedule-linked-list and frees memory
void task_terminate(task_t* task) {
    pic_mask_irq(0); // dont switch tasks anymore
    if (task == NULL) {
        return;
    }
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
    free(task, sizeof(task_t) / 4096); // sizeof(task_t) is 8192 bytes
    pic_unmask_irq(0);
}

task_t* task_get_ptr_by_pid(uint32_t pid) {
    task_t* task = current_task;
    while (task->pid != pid) {
        task = task->next;
    }
    if (task->pid == pid) {
        return task;
    }
    return NULL;
}
task_t* task_get_ptr_by_parent_pid(uint32_t pid) {
    task_t* task = current_task;
    while (task->parent_pid != pid) {
        task = task->next;
    }
    if (task->parent_pid == pid) {
        return task;
    }
    return NULL;
}

void task_kill(uint32_t pid) {
    asm("cli");
    task_t* task = task_get_ptr_by_pid(pid);
    task_terminate(task);
    bool nothing_left = false;
    while (!nothing_left) {
        task = task_get_ptr_by_parent_pid(pid);
        task_terminate(task);
        if (task == NULL) {
            nothing_left = true;
        }
    }
    asm("sti");
}

// returned to at the end of a task, sets its state to terminated and waits
void task_exit() {
    schedule_set_task_terminated();
    for (;;) {
        asm("hlt");
    }
}

// allocates memory for task and sets its stack up
task_t* task_create(void* function) {
    task_t* new_task = (task_t*)malloc(sizeof(task_t) / 4096); // sizeof(task_t) = 8192
    memset(new_task->stack, 0, TASK_STACK_SIZE);
    new_task->rsp =
        &(new_task->stack[TASK_STACK_SIZE -
                          21]); // 15 regs for poping in task_switch, 5 for return address
    new_task->stack[TASK_STACK_SIZE - 1] = &task_exit;
    new_task->stack[TASK_STACK_SIZE - 2] = 0x30; // return SS
    new_task->stack[TASK_STACK_SIZE - 3] =
        &(new_task->stack[TASK_STACK_SIZE - 1]);     // return RSP
    new_task->stack[TASK_STACK_SIZE - 4] = 0x203;    //   return RFLAGS
    new_task->stack[TASK_STACK_SIZE - 5] = 0x28;     // return cs
    new_task->stack[TASK_STACK_SIZE - 6] = function; // return address -> rip
    new_task->stack[TASK_STACK_SIZE - 7] =
        &(new_task->stack[TASK_STACK_SIZE - 1]); // rbp popped manually
                                                 // TODO: add stack needed for iretq
    return new_task;
}

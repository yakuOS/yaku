#include "schedule.h"
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <interrupts/pic.h>
static task_t* current_task;
task_t* scheduler_get_current_task() {
    return current_task;
}

// put tasks inbetween linked-task-list
void schedule_task(task_t* task) {
    if (current_task == NULL) {
        current_task = task;
        current_task->next = task;
    } else {
        task->next = current_task->next;
        current_task->next = task;
    }
}

void schedule_set_task_terminated() {
    current_task->task_state = TASK_STATE_TERMINATED;
}

// set current task sleeping and wait till next tick to switch
void schedule_sleep(uint32_t ticks) {
    task_sleep(current_task, ticks);
    timer_sleep_ticks(ticks);
}

static enum task_priority task_repetition =
    TASK_PRIORITY_LOW; // counter for task repition
void schedule_switch(uint64_t* rsp) {
    if (current_task == NULL || task_repetition++ < current_task->priority) {
        return;
    }

    task_repetition = TASK_PRIORITY_LOW;
    task_t* old_task = current_task;
    current_task = current_task->next;
    if (old_task->task_state != TASK_STATE_WAITING) {
        old_task->rsp = rsp;
    }
    if (current_task->task_state == TASK_STATE_WAITING) {
        current_task->task_state = TASK_STATE_RUNNING;
    }
    if (current_task->task_state == TASK_STATE_TERMINATED) {
        task_t* task_to_terminate = current_task;
        current_task = current_task->next;
        if (current_task->next == current_task) {
            task_terminate(task_to_terminate, old_task);
            current_task = NULL;
            return;
        }
        task_terminate(task_to_terminate, old_task);
    }

    uint16_t counter = TASKS_MAX + 1;
    while (current_task->task_state == TASK_STATE_PAUSED) {
        current_task = current_task->next;
        if (counter-- <= 0) {
            serial_printf("Exception: All tasks paused!\n");
            current_task = NULL;
            return;
        }
    }
    while (current_task->task_state == TASK_STATE_SLEEP && counter-- > 0) {
        if (current_task->sleep_till > pit_tick_get()) {
            current_task = current_task->next;
        } else {
            current_task->task_state = TASK_STATE_RUNNING;
            current_task->sleep_till = 0;
        }
    }
    pic_send_eoi(0);
    switch_to_task(current_task->rsp);
}

void schedule_init(void* kernel_function) {
    pic_mask_irq(0);
    task_add(kernel_function, TASK_PRIORITY_LOW, 0);
    pic_unmask_irq(0);
}

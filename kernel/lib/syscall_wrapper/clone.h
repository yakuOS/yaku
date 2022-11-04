#pragma once
#include "syscalls.h"

struct task_parameters_l {
    uint64_t first; // rdi
    uint64_t second; // rsi
    uint64_t third; // rdx
    uint64_t fourth; // rcx
    uint64_t fifth; // r8
    uint64_t sixth; // r9
} task_parameters_l;

static inline uint64_t clone(void* function, task_parameters_l* parameters) {
    return syscall(SYS_create_task, function, parameters);
}

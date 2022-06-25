#pragma once
#include <types.h>
#include <multitasking/task.h>
#include <lib/syscall_wrapper/syscalls.h>


uint64_t isr_syscall(uint64_t rid, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

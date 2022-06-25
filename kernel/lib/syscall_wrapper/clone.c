#include "syscalls.h"
#include <drivers/serial.h>
#include <multitasking/task.h>
void clone(void* function, task_parameters_t* parameters) {
    serial_printf("clone\n");
    serial_printf("return %lu \n",syscall(SYS_create_task, function, parameters));
}

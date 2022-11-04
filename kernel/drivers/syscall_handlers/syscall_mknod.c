#include <memory/pmm.h>
#include <virtual_fs/virtual_fs.h>
#include <virtual_fs/FILE.h>
#include <drivers/serial.h>
#include <lib/stat.h>

int64_t syscall_mknod(const char* pathname, mode_t mode, dev_t dev){
    serial_printf("syscall_mknod: pathname=%s\n", pathname);
    return virtual_fs_mknod(pathname, mode, dev);
}

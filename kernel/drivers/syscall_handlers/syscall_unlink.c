#include <memory/pmm.h>
#include <virtual_fs/virtual_fs.h>
#include <virtual_fs/FILE.h>
#include <drivers/serial.h>
#include <lib/stat.h>

int64_t syscall_unlink(const char* pathname){
    return virtual_fs_unlink(pathname);
}

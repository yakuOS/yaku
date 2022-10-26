kernel_asm_srcs = files([
    'interrupts/isr.asm',
    'interrupts/idt.asm',
    'drivers/sse.asm',
    'multitasking/context_switch.asm',
    'drivers/lba/lba.asm',
])

kernel_c_srcs = files([
    'kernel.c',
    'drivers/vga_text.c',
    'drivers/pit.c',
    'drivers/timer.c',
    'drivers/serial.c',
    'drivers/input/ps2.c',
    'drivers/rtc.c',
    'drivers/fpu.c',
    'drivers/input/input_device.c',
    'drivers/fb.c',
    'interrupts/isr.c',
    'interrupts/idt.c',
    'interrupts/pic.c',
    'resources/keyboard_keymap.c',
    'resources/keyboard_stringmap.c',
    'resources/tbz_logo.c',
    'resources/yaku_logo.c',
    'resources/cursor.c',
    'resources/font8x8.c',
    'lib/input/input_event.c',
    'lib/input/keyboard_handler.c',
    'lib/input/mouse_handler.c',
    'lib/datetime.c',
    'memory/pmm.c',
    'runtime/windowmanager.c',
    'runtime/runtime.c',
    'runtime/drawutils.c',
    'runtime/programs/tbz.c',
    'runtime/programs/cube.c',
    'runtime/programs/gradient.c',
    'runtime/programs/editor.c',
    'multitasking/task.c',
    'multitasking/scheduler.c',
    'drivers/lba/lba.c',
    'lib/atoi.c',
    'lib/getopt.c',
    'lib/uuid.c',
    'lib/write_to_drive.c',
    'echfs/mkfs.echfs.c',
    'echfs/echfs-fuse.c',
    'echfs/part.c',
    'lib/file.c',
    'virtual_fs/virtual_fs.c',
    'drivers/syscall_handlers/syscall.c'
])

kernel_asmflags = ['-f', 'elf64']
kernel_ccflags = ['-c', '-m64', '-nostdlib', '-fno-builtin', '-ffreestanding', '-fno-stack-protector', '-mno-red-zone']

kernel_asm_objs = as_gen.process(kernel_asm_srcs, extra_args: kernel_asmflags)

subdir('thirdparty')

kernel_include_flags = []
foreach include_dir : kernel_third_party_include_dirs
    kernel_include_flags += ['-I', include_dir]
endforeach
kernel_include_flags += ['-I', meson.current_source_dir()]
kernel_include_flags += ['-I', meson.current_source_dir() + '/misc']
kernel_include_flags += ['-I', meson.current_source_dir() + '/lib']

kernel_c_objs = cc_gen.process(kernel_c_srcs,
    extra_args: [kernel_ccflags, kernel_include_flags]
)

kernel_ld_script = files('link.ld')

kernel_target = custom_target('kernel',
    output: 'kernel.elf',
    input: [kernel_asm_objs, kernel_c_objs, kernel_third_party_objs],
    command: [ld, '-T', kernel_ld_script, '-o', '@OUTPUT@', '@INPUT@']
)

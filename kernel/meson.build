kernel_asm_srcs = files([
    'boot/boot32.asm',
    'boot/boot64.asm',
    'boot/multiboot.asm',
    'interrupts/isr.asm',
    'interrupts/idt.asm'
])

kernel_c_srcs = files([
    'kernel.c',
    'drivers/vga_text.c',
    'drivers/pit.c',
    'drivers/timer.c',
    'drivers/serial.c',
    'drivers/input/ps2.c',
    'interrupts/isr.c',
    'interrupts/idt.c',
    'interrupts/pic.c'
])

kernel_asmflags = ['-f', 'elf64']
kernel_ccflags = ['-c', '-m64', '-nostdlib', '-fno-builtin', '-ffreestanding', '-fno-stack-protector']

kernel_asm_objs = as_gen.process(kernel_asm_srcs, extra_args: kernel_asmflags)

subdir('thirdparty')

kernel_include_flags = []
foreach include_dir : kernel_third_party_include_dirs
    kernel_include_flags += ['-I', include_dir]
endforeach
kernel_include_flags += ['-I', meson.current_source_dir()]
kernel_include_flags += ['-I', meson.current_source_dir() + '/misc']

kernel_c_objs = cc_gen.process(kernel_c_srcs,
    extra_args: [kernel_ccflags, kernel_include_flags]
)

kernel_ld_script = files('link.ld')

kernel_target = custom_target('kernel',
    output: 'kernel.bin',
    input: [kernel_asm_objs, kernel_c_objs, kernel_third_party_objs],
    command: [ld, '-m', 'elf_x86_64', '-T', kernel_ld_script, '-o', '@OUTPUT@', '@INPUT@']
)

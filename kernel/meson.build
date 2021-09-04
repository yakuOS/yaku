kernel_asm_srcs = files([
    'boot.asm',
    'main.asm'
])

kernel_c_srcs = files([
    'main.c',
    'terminal.c'
])

kernel_asm_objs = as_gen.process(kernel_asm_srcs, extra_args: ['-f', 'elf64'])
kernel_c_objs = cc_gen.process(kernel_c_srcs,
    extra_args: ['-c', '-m64', '-nostdlib', '-fno-builtin', '-ffreestanding', '-fno-stack-protector']
)

kernel_ld_script = files('link.ld')

kernel_target = custom_target('kernel',
    output: 'kernel.bin',
    input: [kernel_asm_objs, kernel_c_objs],
    command: [ld, '-m', 'elf_x86_64', '-T', kernel_ld_script, '-o', '@OUTPUT@', '@INPUT@']
)

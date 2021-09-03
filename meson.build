project('yaku')

as = find_program('nasm')
ld = find_program('ld')
cc = find_program('gcc')

mkrescue = find_program('grub-mkrescue')

as_gen = generator(as,
    output: '@PLAINNAME@.o',
    arguments : ['@INPUT@', '-o', '@OUTPUT@', '@EXTRA_ARGS@']
)

cc_gen = generator(cc,
    output: '@PLAINNAME@.o',
    arguments: ['@INPUT@', '-o', '@OUTPUT@', '-Wall', '-Wextra', '@EXTRA_ARGS@']
)

subdir('kernel')

iso_target = custom_target('iso',
    output: 'yaku.iso',
    command: ['sh', '@SOURCE_ROOT@/make_iso.sh', '@OUTPUT@', kernel_target, '@PRIVATE_DIR@', '@SOURCE_ROOT@']
)

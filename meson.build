project('yaku')

as = find_program('nasm')
ld = find_program('ld')
cc = find_program('gcc')


qemu = find_program('qemu-system-x86_64', required: false)

as_gen = generator(as,
    output: '@PLAINNAME@.o',
    arguments : ['@INPUT@', '-o', '@OUTPUT@', '@EXTRA_ARGS@']
)

cc_gen = generator(cc,
    output: '@PLAINNAME@.o',
    arguments: ['@INPUT@', '-o', '@OUTPUT@', '-Wall', '-Wextra', '@EXTRA_ARGS@']
)

subdir('kernel')

limine_target = custom_target('limine',
    output: 'limine',
    command: [
        'sh', '@SOURCE_ROOT@/make_limine.sh', '@OUTPUT@',
    ]
)

iso_target = custom_target('iso',
    output: 'yaku.iso',
    command: ['sh', '@SOURCE_ROOT@/make_iso.sh', '@OUTPUT@', kernel_target, '@PRIVATE_DIR@', '@SOURCE_ROOT@', limine_target]
)

if qemu.found()
    qemu_run_target = run_target('qemu-run',
        command: [qemu, '-cdrom', iso_target, '-drive', 'file=sata0.img,index=0,media=disk,format=raw', '-m', '1024M', '-enable-kvm']
    )
else
    warning('the \'qemu-run\' target is not available because qemu could not be found')
endif

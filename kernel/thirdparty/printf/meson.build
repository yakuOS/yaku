kernel_third_party_printf_c_srcs = files([
    'printf.c'
])

kernel_third_party_printf_objs = cc_gen.process(
    kernel_third_party_printf_c_srcs,
    extra_args: kernel_ccflags
)

kernel_third_party_printf_include_dirs = [meson.current_source_dir()]

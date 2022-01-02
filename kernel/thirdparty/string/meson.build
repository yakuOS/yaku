kernel_third_party_string_c_srcs = files([
    'strchr.c','strcpy.c','memcmp.c','strlen.c','strncat.c','memchr.c','strnstr.c','strxfrm.c','memrchr.c','strchrnul.c','strnlen.c','strtok.c','strncmp.c','memmove.c','strcmp.c','strpbrk.c', 'strstr.c','strncpy.c','memcpy.c','strcat.c','strspn.c','strcoll.c','memset.c','strerror.c', 'strrchr.c','memmem.c','strcspn.c'
])

kernel_third_party_string_objs = cc_gen.process(
    kernel_third_party_string_c_srcs,
    extra_args: kernel_ccflags
)

kernel_third_party_string_include_dirs = [meson.current_source_dir()]

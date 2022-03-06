# this script is intended to be called by meson during the build
# of the 'iso' target and shouldn't be invoked manually

OUTPUT=$1
KERNEL_BIN=$2
PRIVATE_DIR=$3
SRC_ROOT=$4
LIMINE_DIR=$5


mkdir -p ${PRIVATE_DIR}
cp ${SRC_ROOT}/limine.cfg ${KERNEL_BIN} ${LIMINE_DIR}/limine-cd.bin \
    ${LIMINE_DIR}/limine.sys ${LIMINE_DIR}/limine-eltorito-efi.bin \
    ${PRIVATE_DIR}

xorriso -as mkisofs -b limine-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot limine-eltorito-efi.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    ${PRIVATE_DIR} -o ${OUTPUT}

${LIMINE_DIR}/limine-install ${OUTPUT}

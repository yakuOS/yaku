# this script is intended to be called by meson during the build
# of the 'iso' target and shouldn't be invoked manually

OUTPUT=$1
KERNEL_BIN=$2
PRIVATE_DIR=$3
SRC_ROOT=$4


mkdir -p ${PRIVATE_DIR}/boot/grub
cp ${SRC_ROOT}/grub.cfg ${PRIVATE_DIR}/boot/grub
cp ${KERNEL_BIN} ${PRIVATE_DIR}/boot
grub-mkrescue -o ${OUTPUT} ${PRIVATE_DIR}
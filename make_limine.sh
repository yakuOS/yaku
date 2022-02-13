OUTPUT=$1

git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1 ${OUTPUT} && \
make -C ${OUTPUT}

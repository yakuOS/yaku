#include <lib/stdio.h>
#include <lib/origin.h>
#include <lib/atoi.h>
#include <string.h>
#include <types.h>
#define RESERVED_BLOCKS 16

// boot sector code in boot.asm
const uint8_t _binary_boot_bin_start[];

static FILE* image;

static inline void wr_qword(uint64_t loc, uint64_t x) {
    fseek(image, (long)loc, SEEK_SET);
    fwrite(&x, 8, 1, image);
    return;
}
static inline void wr_blocks(uint64_t loc, uint8_t blocks, uint8_t* x) {
    fseek(image, (long)loc, SEEK_SET);
    fwrite(x, 512, blocks, image);
    return;
}

int echfs_mkfs_main(int argc, char** argv) {
    const uint8_t* boot_sector = _binary_boot_bin_start;
    
    if (argc < 4) {
        serial_printf(
            "%s: usage: %s <image> <bytes per block> <reserved blocks factor>\n", argv[0],
            argv[0]);
        return 1;
    }
    image = fopen("/lba_drive/first_drive", NULL);
    if (image == -1) {
        serial_printf("%s: error: no valid image specified.\n", argv[0]);
    }

    fseek(image, 0L, SEEK_END);
    uint64_t imgsize = (uint64_t)ftell(image);
    rewind(image);
    fclose(image);
    image = fopen("/lba_drive/first_drive", NULL);


    uint64_t bytesperblock = atoi(argv[2]);
    serial_printf("%s: info: bytes per block: %lu\n", argv[0], bytesperblock);
    if ((bytesperblock <= 0) || (bytesperblock % 512)) {
        serial_printf("%s: error: block size MUST be a multiple of 512.\n", argv[0]);
        fclose(image);
        return 1;
    }

    if (imgsize % bytesperblock) {
        serial_printf("%s: error: image is not block-aligned.\n", argv[0]);
        fclose(image);
        return 1;
    }

    uint64_t reserved_factor = atoi(argv[3]);

    if ((reserved_factor <= 0) || (reserved_factor >= 100)) {
        serial_printf("%s: error: reserved blocks factor must be between 1%% and 99%%\n",
                      argv[0]);
        fclose(image);
        return 1;
    }

    uint64_t blocks = imgsize/bytesperblock;

    // fseek(image, 0, SEEK_SET);
    // fwrite(boot_sector, 512, 1, image);

    fseek(image, 4, SEEK_SET);
    fputs("_ECH_FS_", image);
    wr_qword(12, blocks);                           // blocks
    serial_printf("reserved blocks: %lu\n", blocks / (100 / reserved_factor));
    wr_qword(20, blocks / (100 / reserved_factor)); // reserved blocks
    wr_qword(28, bytesperblock);                    // block size
    // mark reserved blocks
    uint64_t loc = RESERVED_BLOCKS * bytesperblock;

    uint64_t fatsize = (blocks * sizeof(uint64_t)) / bytesperblock;
    uint64_t dirsize = blocks / (100 / reserved_factor);
    uint64_t buffer[64];
    for (uint64_t i = 0; i < 64; i++) {
        buffer[i] = 0xfffffffffffffff0;
    }
    serial_printf("%s: info: writing FAT...\n", argv[0]);
    serial_printf("%lu", (RESERVED_BLOCKS + fatsize + dirsize));
    for (uint64_t i = 0; i < (RESERVED_BLOCKS + fatsize + dirsize); i+=64) {
        // wr_qword(loc, 0xfffffffffffffff0);
        wr_blocks(loc, 1, (uint8_t*)buffer);
        loc += sizeof(uint64_t)*64;
        if (i %100000 == 0) {
            serial_printf("progress formation: %d%%\n", (i*100)/(RESERVED_BLOCKS + fatsize + dirsize));
        }
    }
    fflush(image);
    fclose(image);
    serial_printf("%s: info: formatting complete.\n", argv[0]);
    return 0;
}

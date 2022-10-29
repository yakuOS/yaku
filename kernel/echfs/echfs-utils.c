#include <lib/atoi.h>
#include <lib/getopt.h>
#include <lib/stat.h>
#include <lib/uuid.h>
#include <memory/pmm.h>
#include <string.h>
#include <types.h>
#include <lib/stdio.h>
#include <lib/stat.h>
#include "part.h"
#define PRIu64 __PRI64_PREFIX "u"
#define EXIT_FAILURE 1 /* Failing exit status.  */
#define EXIT_SUCCESS 0
#define SEARCH_FAILURE 0xffffffffffffffff
#define ROOT_ID 0xffffffffffffffff
#define BYTES_PER_SECT 512
#define SECTORS_PER_BLOCK 1
#define BYTES_PER_BLOCK (SECTORS_PER_BLOCK * BYTES_PER_SECT)
#define ENTRIES_PER_SECT 2
#define ENTRIES_PER_BLOCK (SECTORS_PER_BLOCK * ENTRIES_PER_SECT)
#define FILENAME_LEN 201
#define RESERVED_BLOCKS 16
#define FILE_TYPE 0
#define DIRECTORY_TYPE 1
#define DELETED_ENTRY 0xfffffffffffffffe
#define RESERVED_BLOCK 0xfffffffffffffff0
#define END_OF_CHAIN 0xffffffffffffffff

typedef struct {
    uint64_t parent_id;
    uint8_t type;
    char name[FILENAME_LEN];
    uint64_t atime;
    uint64_t mtime;
    uint16_t perms;
    uint16_t owner;
    uint16_t group;
    uint64_t ctime;
    uint64_t payload;
    uint64_t size;
} __attribute__((packed)) entry_t;

typedef struct {
    uint64_t target_entry;
    entry_t target;
    entry_t parent;
    char name[FILENAME_LEN];
    int failure;
    int not_found;
} path_result_t;
static int verbose = 1;
static int mbr = 0;
static int gpt = 0;
static int part = 0;
static int force = 0;

static FILE* image;
static uint64_t part_offset;
static uint64_t imgsize;
static uint64_t blocks;
static uint64_t fatsize;
static uint64_t fatstart = RESERVED_BLOCKS;
static uint64_t dirsize;
static uint64_t dirstart;
static uint64_t datastart;
static uint64_t bytesperblock;
inline static int echfs_fseek(FILE* file, long loc, int mode) {
    return fseek(file, loc + part_offset, mode);
}

static inline uint8_t rd_byte(uint64_t loc) {
    echfs_fseek(image, (long)loc, SEEK_SET);
    return (uint8_t)fgetc(image);
}

static inline void wr_byte(uint64_t loc, uint8_t x) {
    echfs_fseek(image, (long)loc, SEEK_SET);
    fputc((int)x, image);
    return;
}

static inline uint16_t rd_word(uint64_t loc) {
    uint16_t x = 0;
    echfs_fseek(image, (long)loc, SEEK_SET);
    fread(&x, 2, 1, image);
    return x;
}

static inline void wr_word(uint64_t loc, uint16_t x) {
    echfs_fseek(image, (long)loc, SEEK_SET);
    fwrite(&x, 2, 1, image);
    return;
}

static inline uint32_t rd_dword(uint64_t loc) {
    uint32_t x = 0;
    echfs_fseek(image, (long)loc, SEEK_SET);
    fread(&x, 4, 1, image);
    return x;
}

static inline void wr_dword(uint64_t loc, uint32_t x) {
    echfs_fseek(image, (long)loc, SEEK_SET);
    fwrite(&x, 4, 1, image);
    return;
}

static inline uint64_t rd_qword(uint64_t loc) {
    uint64_t x = 0;
    echfs_fseek(image, (long)loc, SEEK_SET);
    fread(&x, 8, 1, image);
    return x;
}

static inline void wr_qword(uint64_t loc, uint64_t x) {
    echfs_fseek(image, (long)loc, SEEK_SET);
    fwrite(&x, 8, 1, image);
    return;
}

static inline void rd_entry(entry_t* res, uint64_t entry) {
    uint64_t loc = (dirstart * bytesperblock) + (entry * sizeof(entry_t));

    if (loc >= (dirstart + dirsize) * bytesperblock) {
        serial_printf("PANIC! ATTEMPTING TO READ DIRECTORY OUT OF BOUNDS!\n");
        return;
    }

    echfs_fseek(image, (long)loc, SEEK_SET);
    fread(res, sizeof(entry_t), 1, image);
}

static inline void wr_entry(uint64_t entry, entry_t* entry_src) {
    uint64_t loc = (dirstart * bytesperblock) + (entry * sizeof(entry_t));

    if (loc >= (dirstart + dirsize) * BYTES_PER_BLOCK) {
        serial_printf("PANIC! ATTEMPTING TO WRITE DIRECTORY OUT OF BOUNDS!\n");
        return;
    }

    echfs_fseek(image, (long)loc, SEEK_SET);
    uint8_t* buf = (uint8_t*)entry_src;
    serial_printf("writing to position %lu\n", buf[1]);
    fwrite(entry_src, sizeof(entry_t), 1, image);
}

static uint64_t import_chain(FILE* source) {
    uint8_t* block_buf = malloc(bytesperblock);
    if (!block_buf) {
        serial_printf("malloc failure");
        return;
    }

    fseek(source, 0L, SEEK_END);
    uint64_t source_size = (uint64_t)ftell(source);
    rewind(source);

    if (!source_size)
        return END_OF_CHAIN;

    uint64_t source_size_blocks = (source_size + bytesperblock - 1) / bytesperblock;

    if (verbose) {
        serial_printf("file size: %u\n", source_size);
        serial_printf("file size in blocks: %u\n", source_size_blocks);
    }

    uint64_t* blocklist = malloc((source_size_blocks * sizeof(uint64_t)-1)/4096+1);
    if (!blocklist) {
        serial_printf("malloc failure");
        return;
    }

    echfs_fseek(image, fatstart * bytesperblock, SEEK_SET);
    uint64_t block = 0;
    for (uint64_t i = 0; i < source_size_blocks; i++) {
        uint64_t vvv;
        for (; fread(&vvv, sizeof(uint64_t), 1, image), vvv; block++)
            ;
        blocklist[i] = block++;
    }

    for (uint64_t i = 0; i < source_size_blocks; i++) {
        echfs_fseek(image, blocklist[i] * bytesperblock, SEEK_SET);

        // copy block
        fwrite(block_buf, 1, fread(block_buf, 1, bytesperblock, source), image);
    }

    for (uint64_t i = 0;; i++) {
        echfs_fseek(image, fatstart * bytesperblock + blocklist[i] * sizeof(uint64_t),
                    SEEK_SET);
        if (i == source_size_blocks - 1) {
            uint64_t vvv = END_OF_CHAIN;
            fwrite(&vvv, sizeof(uint64_t), 1, image);
            break;
        }
        fwrite(&blocklist[i + 1], sizeof(uint64_t), 1, image);
    }

    block = blocklist[0];

    free(blocklist);
    free(block_buf);
    return block;
}

static void export_chain(FILE* dest, entry_t src) {
    uint64_t cur_block;
    uint8_t* block_buf = malloc((bytesperblock-1)/4096+1);
    if (!block_buf) {
        serial_printf("malloc failure");
        return;
    }

    for (cur_block = src.payload; cur_block != END_OF_CHAIN;) {
        echfs_fseek(image, (long)(cur_block * bytesperblock), SEEK_SET);
        // copy block
        if (((uint64_t)ftell(dest) + bytesperblock) >= src.size) {
            fread(block_buf, src.size % bytesperblock, 1, image);
            fwrite(block_buf, src.size % bytesperblock, 1, dest);
            break;
        } else {
            fread(block_buf, bytesperblock, 1, image);
            fwrite(block_buf, bytesperblock, 1, dest);
        }

        cur_block = rd_qword((fatstart * bytesperblock) + (cur_block * sizeof(uint64_t)));
    }

    free(block_buf);
    return;
}

static void delete_chain(uint64_t payload) {
    if (payload != END_OF_CHAIN) {
        uint64_t block = payload;
        for (;;) {
            uint64_t next_block =
                rd_qword((fatstart * bytesperblock) + (block * sizeof(uint64_t)));
            wr_qword((fatstart * bytesperblock) + (block * sizeof(uint64_t)), 0);
            if (next_block == END_OF_CHAIN)
                break;
            block = next_block;
        }
    }
}

static uint64_t search(const char* name, uint64_t parent, uint8_t type) {
    // returns unique entry #, SEARCH_FAILURE upon failure/not found
    uint64_t loc = (dirstart * bytesperblock);
    echfs_fseek(image, (long)loc, SEEK_SET);
    for (uint64_t i = 0;; i++) {
        entry_t entry;
        fread(&entry, sizeof(entry_t), 1, image);
        if (!entry.parent_id)
            return SEARCH_FAILURE; // check if past last entry
        if (i >= (dirsize * ENTRIES_PER_BLOCK))
            return SEARCH_FAILURE; // check if past directory table
        if ((entry.parent_id == parent) && (entry.type == type) &&
            (!strcmp(entry.name, name)))
            return i;
    }
}

static path_result_t path_resolver(const char* path, uint8_t type) {
    // returns a struct of useful info
    // failure flag set upon failure
    // not_found flag set upon not found
    // even if the file is not found, info about the "parent"
    // directory and name are still returned
    char name[FILENAME_LEN];
    entry_t parent = {0};
    int last = 0;
    int i;
    path_result_t result;
    entry_t empty_entry = {0};

    result.name[0] = 0;
    result.target_entry = 0;
    result.parent = empty_entry;
    result.target = empty_entry;
    result.failure = 0;
    result.not_found = 0;

    parent.payload = ROOT_ID;

    if ((type == DIRECTORY_TYPE) && !strcmp(path, "/")) {
        result.target.payload = ROOT_ID;
        return result; // exception for root
    }
    if ((type == FILE_TYPE) && !strcmp(path, "/")) {
        result.failure = 1;
        return result; // fail if looking for a file named "/"
    }

    if (*path == '/')
        path++;

next:
    for (i = 0; *path != '/'; path++) {
        if (!*path) {
            last = 1;
            break;
        }
        name[i++] = *path;
    }
    name[i] = 0;
    path++;

    if (!last) {
        uint64_t search_res = search(name, parent.payload, DIRECTORY_TYPE);
        if (search_res == SEARCH_FAILURE) {
            result.failure = 1; // fail if search fails
            return result;
        }
        rd_entry(&parent, search_res);
    } else {
        uint64_t search_res = search(name, parent.payload, type);
        if (search_res == SEARCH_FAILURE)
            result.not_found = 1;
        else {
            rd_entry(&result.target, search_res);
            result.target_entry = search_res;
        }
        result.parent = parent;
        strcpy(result.name, name);
        return result;
    }

    goto next;
}

static inline uint64_t get_free_id(void) {
    uint64_t id = 1;
    uint64_t i;

    uint64_t loc = (dirstart * bytesperblock);
    echfs_fseek(image, (long)loc, SEEK_SET);

    for (i = 0;; i++) {
        entry_t entry;
        fread(&entry, sizeof(entry_t), 1, image);
        if (!entry.parent_id)
            break;
        if ((entry.type == 1) && (entry.payload == id))
            id = (entry.payload + 1);
    }

    return id;
}

static void mkdir_cmd(int argc, char** argv) {
    uint64_t i;
    entry_t entry = {0};

    if (argc < 4) {
        serial_printf("%s: %s: missing argument: directory name.\n", argv[0], argv[2]);
        return;
    }

    path_result_t path_result = path_resolver(argv[3], DIRECTORY_TYPE);

    // check if it exists
    if (!(path_result.not_found)) {
        serial_printf("%s: %s: directory `%s` already exists.\n", argv[0], argv[2],
                      argv[3]);
        return;
    }

    // find empty entry
    uint64_t loc = (dirstart * bytesperblock);
    echfs_fseek(image, (long)loc, SEEK_SET);
    for (i = 0;; i++) {
        entry_t entry_i;
        fread(&entry_i, sizeof(entry_t), 1, image);
        if ((entry_i.parent_id == 0) || (entry_i.parent_id == DELETED_ENTRY))
            break;
    }

    entry.parent_id = path_result.parent.payload;
    serial_printf("hallo\n");
    if (verbose)
        serial_printf("new directory's parent ID: %u\n", entry.parent_id);
    entry.type = DIRECTORY_TYPE;
    strcpy(entry.name, path_result.name);
    entry.payload = get_free_id();
    if (verbose)
        serial_printf("new directory's ID: %u\n", entry.payload);
    if (verbose)
        serial_printf("writing to entry #%u\n", i);
    uint64_t tm = 1000000;// @TODO implement utc time instead of static(uint64_t) time(NULL);
    entry.ctime = tm;
    entry.atime = tm;
    entry.mtime = tm;
    entry.perms = 0644; /* TODO: set appropriate permissions somehow */

    wr_entry(i, &entry);

    if (verbose)
        serial_printf("created directory `%s`\n", argv[3]);

    return;
}

static void import_cmd(int argc, char** argv) {
    FILE* source;
    entry_t entry = {0};
    uint64_t i;

    if (argc < 4) {
        serial_printf("%s: %s: missing argument: source file.\n", argv[0], argv[2]);
        return;
    }
    if (argc < 5) {
        serial_printf("%s: %s: missing argument: destination file.\n", argv[0], argv[2]);
        return;
    }

    // struct stat s;
    // stat(argv[3], &s);
    // if (!S_ISREG(s.st_mode)) { TODO: implement this
    //     serial_printf("%s: warning: source file `%s` is not a regular file, exiting.\n",
    //                   argv[0], argv[3]);
    //     return;
    // }

    // make directory
    if (path_resolver(argv[4], FILE_TYPE).failure) {
        char newdirname[4096];
        int i = 0;
    subdir:
        for (;; i++) {
            if (argv[4][i] == '/')
                break;
            newdirname[i] = argv[4][i];
        }
        newdirname[i] = 0;
        char* oldargv3 = argv[3];
        argv[3] = newdirname;
        mkdir_cmd(argc, argv);
        argv[3] = oldargv3;
        if (path_resolver(argv[4], FILE_TYPE).failure) {
            newdirname[i++] = '/';
            goto subdir;
        }
    }

    path_result_t path_result = path_resolver(argv[4], FILE_TYPE);
    // check if the file exists
    if (!path_result.not_found && !force) {
        serial_printf("%s: %s: error: file `%s` already exists.\n", argv[0], argv[2],
                      argv[4]);
        return;
    }

    if ((source = fopen("/lba_drive/first_drive", "r")) == -1) {
        serial_printf("%s: %s: error: couldn't access `%s`.\n", argv[0], argv[2],
                      argv[3]);
        return;
    }

    uint64_t payload = import_chain(source);
    serial_printf("1\n");
    if (!path_result.not_found) {
        path_result.target.payload = payload;
        // path_result.target.mtime = s.st_mtim.tv_sec;
        wr_entry(path_result.target_entry, &path_result.target);
        fclose(source);
        return;
    }
    serial_printf("2\n");
    entry.parent_id = path_result.parent.payload;
    entry.type = FILE_TYPE;
    strcpy(entry.name, path_result.name);
    entry.payload = payload;
    fseek(source, 0L, SEEK_END);
    entry.size = (uint64_t)ftell(source);
    // entry.ctime = s.st_ctim.tv_sec;
    // entry.atime = s.st_atim.tv_sec;
    // entry.mtime = s.st_mtim.tv_sec;

    // entry.perms = (uint16_t)(s.st_mode & ((1 << 9) - 1));
    serial_printf("3\n");
    // find empty entry
    uint64_t loc = (dirstart * bytesperblock);
    echfs_fseek(image, (long)loc, SEEK_SET);
    for (i = 0;; i++) {
        entry_t entry_i;
        fread(&entry_i, sizeof(entry_t), 1, image);
        if ((entry_i.parent_id == 0) || (entry_i.parent_id == DELETED_ENTRY))
            break;
    }
    serial_printf("4\n");
    wr_entry(i, &entry);
    serial_printf("5\n");
    fclose(source);
    if (verbose)
        serial_printf("imported file `%s` as `%s`\n", argv[3], argv[4]);
    return;
}

static void export_cmd(int argc, char** argv) {
    FILE* dest;

    if (argc < 4) {
        serial_printf("%s: %s: missing argument: source file.\n", argv[0], argv[2]);
        return;
    }
    if (argc < 5) {
        serial_printf("%s: %s: missing argument: destination file.\n", argv[0], argv[2]);
        return;
    }

    path_result_t path_result = path_resolver(argv[3], FILE_TYPE);

    // check if the file doesn't exist
    if (path_result.not_found) {
        serial_printf("%s: %s: error: file `%s` not found.\n", argv[0], argv[2], argv[3]);
        return;
    }

    if ((dest = fopen("/lba_drive/first_drive","w")) == -1) {
        serial_printf("%s: %s: error: couldn't access `%s`.\n", argv[0], argv[2],
                      argv[4]);
        return;
    }

    export_chain(dest, path_result.target);

    fclose(dest);
    if (verbose)
        serial_printf("exported file `%s` as `%s`\n", argv[3], argv[4]);
    return;
}

static void ls_cmd(int argc, char** argv) {
    uint64_t id;

    if (argc < 4)
        id = ROOT_ID;
    else {
        path_result_t result = path_resolver(argv[3], DIRECTORY_TYPE);
        if (result.not_found) {
            serial_printf("%s: %s: error: invalid directory `%s`.\n", argv[0], argv[2],
                          argv[3]);
            return;
        } else
            id = result.target.payload;
    }

    if (verbose)
        serial_printf("  ---- ls ----\n");

    entry_t entryy;
    uint64_t entry = 0;
    while (rd_entry(&entryy, entry++), entryy.parent_id != 0) {
        if (entryy.parent_id != id)
            continue;
        if (entryy.type == DIRECTORY_TYPE)
            serial_printf("[");
        serial_printf("%s", entryy.name);
        if (entryy.type == DIRECTORY_TYPE)
            serial_printf("]");
        serial_printf("\n");
    }

    return;
}

static void format_pass1(int argc, char** argv, int quick) {

    if (argc <= 3) {
        serial_printf("%s: error: unspecified block size.\n", argv[0]);
        fclose(image);
        return;
    }

    if (verbose)
        serial_printf("formatting...\n");

    bytesperblock = 512;

    if ((bytesperblock <= 0) || (bytesperblock % 512)) {
        serial_printf("%s: error: block size MUST be a multiple of 512.\n", argv[0]);
        fclose(image);
        return;
    }

    if (imgsize % bytesperblock) {
        serial_printf("%s: error: image is not block-aligned.\n", argv[0]);
        fclose(image);
        return;
    }

    blocks = imgsize / bytesperblock;

    // write signature
    echfs_fseek(image, 4, SEEK_SET);
    fputs("_ECH_FS_", image);
    // total blocks
    wr_qword(12, blocks);
    // directory size
    wr_qword(20, blocks / 20); // blocks / 20 (roughly 5% of the total)
    // block size
    wr_qword(28, bytesperblock);

    // write UUID
    uuid_t uuid;
    uuid_generate_random(uuid);
    wr_qword(40, ((uint64_t*)uuid)[0]);
    wr_qword(48, ((uint64_t*)uuid)[1]);

    char uuid_str[37];
    uuid_unparse_lower(uuid, uuid_str);
    serial_printf("%s\n", uuid_str);

    if (!quick) {
        echfs_fseek(image, (RESERVED_BLOCKS * bytesperblock), SEEK_SET);
        if (verbose)
            serial_printf("zeroing");

        // zero out the rest of the image
        uint8_t* zeroblock = calloc(bytesperblock, 1);
        if (!zeroblock) {
            serial_printf("calloc failure");
            return;
        }
        for (uint64_t i = (RESERVED_BLOCKS * bytesperblock); i < imgsize;
             i += bytesperblock) {
            fwrite(zeroblock, bytesperblock, 1, image);
            if (verbose)
                serial_printf(".");
        }
        free(zeroblock);

        if (verbose)
            serial_printf("\n");
    }

    return;
}

static void format_pass2(void) {
    // mark reserved blocks
    uint64_t loc = fatstart * bytesperblock;

    for (uint64_t i = 0; i < (RESERVED_BLOCKS + fatsize + dirsize); i++) {
        wr_qword(loc, RESERVED_BLOCK);
        loc += sizeof(uint64_t);
    }

    if (verbose)
        serial_printf("format complete!\n");

    return;
}

int echfs_utils_main(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "vmgfp:")) != -1) {
        switch (opt) {
        case 'v':
            verbose = 1;
            break;
        case 'm':
            mbr = 1;
            break;
        case 'g':
            gpt = 1;
            break;
        case 'p':
            part = atoi(optarg);
            break;
        case 'f':
            force = 1;
            break;
        default:
            serial_printf("Usage: %s <opts> [image] <action> <args...>\n", argv[0]);
            break;
        }
    }

    if (argc - optind == 0) {
        serial_printf("Usage: %s <opts> [image] <action> <args...>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    if ((image = fopen("/lba_drive/first_drive", W)) == -1) {
        serial_printf("%s: error: couldn't access %s.\n", argv[0], argv[optind]);
        return EXIT_FAILURE;
    }

    if (mbr) {
        struct part p;
        mbr_get_part(&p, image, part);
        part_offset = p.first_sect * 512;
        imgsize = p.sect_count * 512;
    } else if (gpt) {
        struct part p;
        gpt_get_part(&p, image, part);
        part_offset = p.first_sect * 512;
        imgsize = p.sect_count * 512;
    } else {
        part_offset = 0;
        fseek(image, 0L, SEEK_END);
        imgsize = (uint64_t)ftell(image);
        rewind(image);
    }

    argv[optind - 1] = argv[0];
    argc -= optind - 1;
    argv += optind - 1;
    if ((argc > 2) && (!strcmp(argv[2], "format")))
        format_pass1(argc, argv, 0);
    if ((argc > 2) && (!strcmp(argv[2], "quick-format")))
        format_pass1(argc, argv, 1);

    char signature[8] = {0};
    echfs_fseek(image, 4, SEEK_SET);
    fread(signature, 8, 1, image);
    if (strncmp(signature, "_ECH_FS_", 8)) {
        serial_printf("%s: error: echidnaFS signature missing.\n", argv[0]);
        fclose(image);
        return EXIT_FAILURE;
    }
    if (verbose)
        serial_printf("echidnaFS signature found\n");

    if (verbose)
        serial_printf("image size: %u bytes\n", imgsize);

    bytesperblock = rd_qword(28);
    if (verbose)
        serial_printf("bytes per block: %u\n", BYTES_PER_BLOCK);

    if (imgsize % bytesperblock) {
        serial_printf("%s: error: image is not block-aligned.\n", argv[0]);
        fclose(image);
        return EXIT_FAILURE;
    }

    blocks = imgsize / bytesperblock;

    if (verbose)
        serial_printf("block count: %u\n", blocks);

    if (verbose)
        serial_printf("declared block count: %u\n", rd_qword(12));
    if (rd_qword(12) != blocks) {
        serial_printf("%s: warning: declared block count mismatch.\n", argv[0]);
    }

    fatsize = (blocks * sizeof(uint64_t)) / bytesperblock;
    if ((blocks * sizeof(uint64_t)) % bytesperblock)
        fatsize++;
    if (verbose)
        serial_printf("expected allocation table size: %u blocks\n", fatsize);

    if (verbose)
        serial_printf("expected allocation table start: block %u\n", fatstart);

    dirsize = rd_qword(20);
    if (verbose)
        serial_printf("declared directory size: %u blocks\n", dirsize);

    dirstart = fatstart + fatsize;
    if (verbose)
        serial_printf("expected directory start: block %u\n", dirstart);

    datastart = RESERVED_BLOCKS + fatsize + dirsize;
    if (verbose)
        serial_printf("expected reserved blocks: %u\n", datastart);

    if (verbose)
        serial_printf("expected usable blocks: %u\n", blocks - datastart);

    if (rd_word(510) == 0xaa55) {
        if (verbose)
            serial_printf("the image is bootable\n");
    } else {
        if (verbose)
            serial_printf("the image is NOT bootable\n");
    }

    if (argc > 2) {
        if (!strcmp(argv[2], "mkdir"))
            mkdir_cmd(argc, argv);
        else if (!strcmp(argv[2], "ls"))
            ls_cmd(argc, argv);
        else if (!strcmp(argv[2], "format"))
            format_pass2();
        else if (!strcmp(argv[2], "quick-format"))
            format_pass2();
        else if (!strcmp(argv[2], "import"))
            import_cmd(argc, argv);
        else if (!strcmp(argv[2], "export"))
            export_cmd(argc, argv);

        else
            serial_printf("%s: error: invalid action: `%s`.\n", argv[0], argv[2]);
    } else
        serial_printf("%s: no action specified, exiting.\n", argv[0]);

    fclose(image);

    return EXIT_SUCCESS;
}

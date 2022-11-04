#include "virtual_fs.h"
#include <drivers/serial.h>
#include <lib/fuse.h>
#include <lib/stat.h>
#include <memory/pmm.h>
#include <string.h>
#include <types.h>

static struct virtual_fs_directory_entry* virtual_fs_root;

struct create_fs_directory_entry_path_result {
    struct virtual_fs_directory_entry* parent;
    char name[MAX_NAME_LENGTH];
};

struct virtual_fs_directory_entry*
virtual_fs_get_directory_entry(struct virtual_fs_directory* directory, char* name) {
    serial_printf("virtual_fs_get_directory_entry: %s\n", name);
    for (uint64_t i = 0;
         i < directory->entries_count;
         i++) {
        serial_printf(
            "entry0.1 %p\n",
            directory->entries[i]);
    }
    for (uint32_t i = 0; i < directory->entries_count; i++) {
        if (strcmp(directory->entries[i].name, name) == 0) {
            return &directory->entries[i];
        }
    }
    return NULL;
}

// fills struct with directory the entry to be created is in and with name of entry to be
// created
struct create_fs_directory_entry_path_result*
create_fs_entry_path_resolver(char* path_2, struct virtual_fs_directory_entry* parent) {
    char* path = (char*)malloc(strlen(path_2) + 1);
    strcpy(path, path_2);
    struct create_fs_directory_entry_path_result* result =
        malloc(sizeof(struct create_fs_directory_entry_path_result));
    memset(result, 0, sizeof(struct create_fs_directory_entry_path_result));
    result->parent = parent;
    if (path[0] == '/') {
        path++;
        result->parent = virtual_fs_root;
    }
    if (result->parent == NULL) {
        result->parent = virtual_fs_root;
    }
    char* name = strtok(path, "/");
    while (name != NULL) {
        struct virtual_fs_directory_entry* entry =
            virtual_fs_get_directory_entry(result->parent->pointer, name);
        if (entry == NULL) {
            break;
        }
        result->parent = entry;
        name = strtok(NULL, "/");
    }
    if (name != NULL) {
        strcpy(result->name, name);
    }
    return result;
}
// fills struct with endpoint pointer, directory of the endpoint and path relative to
// endpoint
struct endpoint_path_result* virtual_fs_endpoint_path_resolver(char* path) {
    for (uint64_t i = 0;
         i < ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries_count;
         i++) {
        serial_printf(
            "entry0 %p\n",
            ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries[i]);
    }
    serial_printf("virtual_fs_endpoint_path_resolver: path=%s\n", path);
    struct endpoint_path_result* result = malloc(sizeof(struct endpoint_path_result));
    memset(result, 0, sizeof(struct endpoint_path_result));
    result->endpoint = NULL;
    result->parent = virtual_fs_root;
    if (path[0] == '/') {
        path++;
    }
    char* name = strtok(path, "/");
    while (name != NULL) {
        serial_printf("virtual_fs_endpoint_path_resolver1: name=%s\n", name);
        struct virtual_fs_directory_entry* entry =
            virtual_fs_get_directory_entry(result->parent->pointer, name);
        if (entry == NULL) {
            serial_printf("virtual_fs_endpoint_path_resolver: entry not found\n");
            break;
        }
        if (entry->type == ENTRY_TYPE_ENDPOINT) {
            serial_printf("virtual_fs_endpoint_path_resolver: endpoint found\n");
            result->endpoint = entry;
            name = strtok(NULL, "/");
            break;
        }
        result->parent = entry;
        name = strtok(NULL, "/");
    }
    serial_printf("virtual_fs_endpoint_path_resolver: name=%s\n", name);
    if (name == 0 && result->endpoint != NULL) {
        strcpy(result->endpoint_path_to_be_passed, "/");
    }
    if (name != NULL) {
        strcpy(result->endpoint_path_to_be_passed, name);
    }
    serial_printf("virtual_fs_endpoint_path_resolver: endpoint_path_to_be_passed=%s\n",
                  result->endpoint_path_to_be_passed);
    return result;
}

uint8_t virtual_fs_add_directory_entry(struct virtual_fs_directory* parent_directory,
                                       char* name, enum entry_type entry_type,
                                       uint64_t* pointer) {
    parent_directory->entries_count++;
    // for (uint64_t i = 0;
    //      i < ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries_count;
    //      i++) {
    //     serial_printf(
    //         "entry0 %s\n",
    //         ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries[i].name);
    // }
    parent_directory->entries =
        realloc(parent_directory->entries, sizeof(struct virtual_fs_directory_entry) *
                                               parent_directory->entries_count);

    struct virtual_fs_directory_entry* entry =
        &parent_directory->entries[parent_directory->entries_count - 1];

    strcpy(entry->name, name);
    entry->type = entry_type;
    entry->pointer = pointer;

    for (uint64_t i = 0;
         i < ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries_count;
         i++) {
        serial_printf(
            "entry0.1 %p\n",
            ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries[i]);
    }
    serial_printf("virtual_fs_add_directory_entry fuse_op open=%p\n", ((struct virtual_fs_endpoint*)entry->pointer)->fuse_ops->open);
    return 0;
}

uint8_t virtual_fs_create_directory(char* path) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, NULL);
    serial_printf("Creating directory %s\n", result->name);
    serial_printf("Parent is %p\n", result->parent);
    if (result->parent == NULL) {
        return 1;
    }
    if (result->name == NULL) {
        return 1;
    }
    if (result->parent->type != ENTRY_TYPE_DIR) {
        return 1;
    }
    if (virtual_fs_get_directory_entry(result->parent->pointer, result->name) != NULL) {
        return 1;
    }
    struct virtual_fs_directory* directory = malloc(sizeof(struct virtual_fs_directory));
    memset(directory, 0, sizeof(struct virtual_fs_directory));
    virtual_fs_add_directory_entry(result->parent->pointer, result->name, ENTRY_TYPE_DIR,
                                   directory);
    free(result);
    return 0;
}
uint8_t virtual_fs_create_endpoint(struct fuse_operations* fuse_operations,
                                   enum endpoint_type endpoint_type, char* path) {

    serial_printf("%s\n", path);
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, NULL);
    serial_printf("parent %s\n", result->parent->name);
    serial_printf("name %s\n", result->name);
    if (result->name == NULL || result->parent == NULL) {
        return 1;
    }
    struct virtual_fs_endpoint* endpoint = malloc(sizeof(struct virtual_fs_endpoint));
    serial_printf("fuse_ops %p\n", fuse_operations->open);
    // memcpy(&endpoint->fuse_ops, fuse_operations, sizeof(struct fuse_operations));
    endpoint->fuse_ops = fuse_operations;

    serial_printf("fuse_ops2 %p\n", endpoint->fuse_ops);
    // serial_printf("fuse_ops %p\n", endpoint->fuse_ops->open);
    endpoint->type = endpoint_type;
    // for (uint64_t i = 0;
    //      i < ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries_count;
    //      i++) {
    //     serial_printf(
    //         "entry1 %s\n",
    //         ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries[i].name);
    // }
    virtual_fs_add_directory_entry(result->parent->pointer, result->name,
                                   ENTRY_TYPE_ENDPOINT, (uint64_t*)endpoint);

    // for (uint64_t i = 0;
    //      i < ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries_count;
    //      i++) {
    //     serial_printf(
    //         "entry %s\n",
    //         ((struct virtual_fs_directory*)virtual_fs_root->pointer)->entries[i].name);
    // }
    free(result);
    // struct endpoint_path_result* result2 = virtual_fs_endpoint_path_resolver(path);
    // struct virtual_fs_endpoint* endpoint2 =
    //     (struct virtual_fs_endpoint*)result2->endpoint->pointer;
    // serial_printf("fuse_ops3 %p\n", endpoint2->fuse_ops->open);


    return 0;
}
uint8_t virtual_fs_remove_endpoint(char* path) {
    struct endpoint_path_result* result = virtual_fs_endpoint_path_resolver(path);
    if (result->endpoint == NULL) {
        return 1;
    }
    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)result->endpoint->pointer;
    if (endpoint->fuse_ops->destroy != NULL) {
        endpoint->fuse_ops->destroy(NULL);
    }
    struct virtual_fs_directory* parent =
        (struct virtual_fs_directory*)result->parent->pointer;
    for (uint32_t i = 0; i < parent->entries_count; i++) {
        if (&parent->entries[i] == result->endpoint) {
            for (uint32_t j = i; j < parent->entries_count - 1; j++) {
                parent->entries[j] = parent->entries[j + 1];
            }
            parent->entries_count--;
            parent->entries =
                realloc(parent->entries, sizeof(struct virtual_fs_directory_entry) *
                                             parent->entries_count);
            free(result->endpoint->pointer);
            free(result);
            return 0;
        }
    }
    free(result);
    return 1;
}
uint8_t virtual_fs_remove_directory_entries(struct virtual_fs_directory* directory) {
    for (uint32_t i = 0; i < directory->entries_count; i++) {
        if (directory->entries[i].type == ENTRY_TYPE_DIR) {
            virtual_fs_remove_directory_entries(directory->entries[i].pointer);
            free(directory->entries[i].pointer);
        }
        if (directory->entries[i].type == ENTRY_TYPE_ENDPOINT) {
            free(directory->entries[i].pointer);
        }
    }
    free(directory->entries);
    return 0;
}
uint8_t virtual_fs_remove_directory(char* path) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, NULL);
    serial_printf("Removing directory %s\n", result->name);
    serial_printf("Parent is %p\n", result->parent);
    if (result->parent == NULL) {
        return 1;
    }
    if (result->name == NULL) {
        return 1;
    }
    if (result->parent->type != ENTRY_TYPE_DIR) {
        return 1;
    }
    if (virtual_fs_get_directory_entry(result->parent->pointer, result->name) == NULL) {
        return 1;
    }
    virtual_fs_remove_directory_entries(result->parent->pointer);
    free(result);
    free(result->parent->pointer);
    return 0;
}

// returns pointer to directory
struct virtual_fs_directory_entry*
virtual_fs_search_directory(char* path, struct virtual_fs_directory_entry* parent) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, parent);
    return result->parent;
}

uint8_t virtual_fs_init() {
    virtual_fs_root = malloc(sizeof(struct virtual_fs_directory_entry));
    strcpy(virtual_fs_root->name, "root");
    virtual_fs_root->type = ENTRY_TYPE_DIR;
    virtual_fs_root->pointer = malloc(sizeof(struct virtual_fs_directory));
    memset(virtual_fs_root->pointer, 0, sizeof(struct virtual_fs_directory));
}

// int virtual_fs_mkdir(const char* path, mode_t mode) {
//     struct path_result* result = virtual_fs_endpoint_path_resolver((char*)path);
//     if (result->endpoint == NULL) {
//         virtual_fs_create_directory((char*)path);
//     } else {
//         ((struct virtual_fs_endpoint*)
//         result->endpoint->pointer)->fuse_ops->mkdir(result->endpoint_path_to_be_passed,
//         mode);
//     }
//     return 0;
// }
// int virtual_fs_rmdir(const char* path) {
//     struct path_result* result = virtual_fs_endpoint_path_resolver((char*)path);
//     if (result->endpoint == NULL) {
//         virtual_fs_remove_directory((char*)path);
//     } else {
//         ((struct virtual_fs_endpoint*)
//         result->endpoint->pointer)->fuse_ops->rmdir(result->endpoint_path_to_be_passed);
//     }
// }

int virtual_fs_open(const char* file_path, struct fuse_file_info* file_info,
                    struct fuse_operations** fuse_ops, char* endpoint_path_buffer) {
    serial_printf("should open %s\n", file_path);
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(file_path);
    serial_printf("endpoint %s\n", path->endpoint->name);
    serial_printf("parent %s\n", path->parent->name);
    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        free(path);
        return;
    }
    serial_printf("endpoint path check 1\n");
    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;
    serial_printf("endpoint path check 1.2 endpoint = %p\n", endpoint->fuse_ops);
    if (endpoint == NULL || endpoint->fuse_ops->open == NULL) {
        serial_printf("endpoint path check 1.3\n");
        free(path);
        serial_printf("endpoint path check 1.4\n");
        return;
    }
    serial_printf("endpoint path check 2\n");
    endpoint->fuse_ops->open(path->endpoint_path_to_be_passed, file_info);
    *fuse_ops = endpoint->fuse_ops;
    strcpy(endpoint_path_buffer, path->endpoint_path_to_be_passed);
    serial_printf("endpoint path check 3\n");
    free(path);
    serial_printf("endpoint path check 4\n");
    return 0;
}

int virtual_fs_opendir(const char* file_path, struct fuse_file_info* file_info,
                       struct fuse_operations** fuse_ops, char* endpoint_path_buffer) {
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(file_path);
    serial_printf("opendir %s\n", file_path);
    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        if (path->parent != NULL) {
            strcpy(endpoint_path_buffer, file_path);
        }
        free(path);
        return;
    }
    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;
    if (endpoint->fuse_ops->opendir == NULL) {
        free(path);
        return;
    }
    serial_printf("opendir %s\n", path->endpoint_path_to_be_passed);
    endpoint->fuse_ops->opendir(path->endpoint_path_to_be_passed, file_info);
    *fuse_ops = endpoint->fuse_ops;
    strcpy(endpoint_path_buffer, path->endpoint_path_to_be_passed);
    free(path);

    return 0;
}

int virtual_fs_create(const char* file_path, mode_t mode,
                      struct fuse_file_info* file_info) {
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(file_path);

    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        free(path);
        return;
    }

    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;

    if (endpoint->fuse_ops->create == NULL) {
        if (endpoint->fuse_ops->mknod != NULL && endpoint->fuse_ops->open != 0) {
            endpoint->fuse_ops->mknod(path->endpoint_path_to_be_passed, mode, 0);
            endpoint->fuse_ops->open(path->endpoint_path_to_be_passed, file_info);
        }
        free(path);
        return;
    }

    endpoint->fuse_ops->create(path->endpoint_path_to_be_passed, mode, file_info);
    free(path);
    return 0;
}

int virtual_fs_unlink(const char* file_path) {
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(file_path);

    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        free(path);
        return;
    }

    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;

    if (endpoint->fuse_ops->unlink == NULL) {
        free(path);
        return;
    }

    endpoint->fuse_ops->unlink(path->endpoint_path_to_be_passed);
    free(path);
    return 0;
}

int virtual_fs_mknod(const char* pathname, mode_t mode, dev_t dev) {
    serial_printf("mknod %s\n", pathname);
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(pathname);
    serial_printf("mknod check 1\n");
    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        free(path);
        return;
    }
    serial_printf("mknod check 2\n");

    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;

    if (endpoint->fuse_ops->mknod == NULL) {
        if (endpoint->fuse_ops->create != NULL) {
            serial_printf("mknod check 3\n");
            struct fuse_file_info* file_info =
                (struct fuse_file_info*)malloc(sizeof(struct fuse_file_info));
            serial_printf("mknod check 4\n");
            endpoint->fuse_ops->create(path->endpoint_path_to_be_passed, mode, file_info);
            serial_printf("mknod check 5\n");
            endpoint->fuse_ops->release(path->endpoint_path_to_be_passed, file_info);
        }
        serial_printf("mknod check 6\n");
        free(path);
        return;
    }
    serial_printf("mknod check 7\n");
    endpoint->fuse_ops->mknod(path->endpoint_path_to_be_passed, mode, dev);
    free(path);
    return 0;
}

int virtual_fs_mkdir(const char* file_path, mode_t mode) {
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(file_path);

    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        free(path);
        return;
    }

    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;

    if (endpoint->fuse_ops->mkdir == NULL) {
        free(path);
        return;
    }

    endpoint->fuse_ops->mkdir(path->endpoint_path_to_be_passed, mode);
    free(path);
    return 0;
}

int virtual_fs_rmdir(const char* file_path) {
    struct endpoint_path_result* path = virtual_fs_endpoint_path_resolver(file_path);

    if (path->parent == NULL || path->endpoint == NULL ||
        path->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        free(path);
        return;
    }

    struct virtual_fs_endpoint* endpoint =
        (struct virtual_fs_endpoint*)path->endpoint->pointer;

    if (endpoint->fuse_ops->rmdir == NULL) {
        free(path);
        return;
    }

    endpoint->fuse_ops->rmdir(path->endpoint_path_to_be_passed);
    free(path);
    return 0;
}

int virtual_fs_readdir(const char* path, void* buffer, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info* fi) {
    struct endpoint_path_result* result = virtual_fs_endpoint_path_resolver(path);
    serial_printf("virtual_fs_readdir: %s\n", path);
    if (result->parent == NULL) {
        free(result);
        return;
    }
    serial_printf("virtual_fs_readdir2: %s\n", result->endpoint_path_to_be_passed);
    if (result->endpoint == NULL || result->endpoint->type != ENTRY_TYPE_ENDPOINT) {
        serial_printf("virtual_fs_readdir3: %s\n", result->endpoint_path_to_be_passed);
        for (int i = 0;
             i < ((struct virtual_fs_directory*)result->parent->pointer)->entries_count;
             i++) {
            serial_printf("virtual_fs_readdir4: %s\n",
                          result->endpoint_path_to_be_passed);
            struct stat st = {
                .st_mode = (((struct virtual_fs_directory*)(result->parent->pointer))
                                    ->entries[i]
                                    .type == ENTRY_TYPE_DIR ||
                            ((struct virtual_fs_endpoint*)((struct virtual_fs_directory*)
                                                               result->parent->pointer))
                                    ->type == ENDPOINT_TYPE_DIRECTORY)
                               ? S_IFDIR
                               : S_IFREG,
            };
            filler(
                buffer,
                ((struct virtual_fs_directory*)result->parent->pointer)->entries[i].name,
                &st, 0, 0);
        }
    }
    return 0;
}
void get_open_pointer_fs(){
    struct endpoint_path_result* result = virtual_fs_endpoint_path_resolver("lba_drive");
    serial_printf("virtual_fs_ get open pointer: %p\n", ((struct virtual_fs_endpoint*)(result->endpoint->pointer))->fuse_ops->open);
}

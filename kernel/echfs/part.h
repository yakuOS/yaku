
#include <types.h>
#include <string.h>
#include <lib/write_to_drive.h>

struct part {
	uint64_t first_sect;
	uint64_t sect_count;
};

int mbr_get_part(struct part *ret, FILE *file, int partition);
int gpt_get_part(struct part *ret, FILE *file, int partition);

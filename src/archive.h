#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <archive.h>
#include <archive_entry.h>

bool hydra_extract(const char *file, const char *path);
int hydra_copydata(struct archive *ar, struct archive *aw);
void hydra_list(const char *file);
void hydra_setup_decompress(struct archive *ar);

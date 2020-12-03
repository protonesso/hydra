#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <archive.h>
#include <archive_entry.h>
#include <brotli/decode.h>

bool hydra_brotlidec(const char *file);
bool hydra_extract(const char *file, const char *path);
int hydra_copydata(struct archive *ar, struct archive *aw);
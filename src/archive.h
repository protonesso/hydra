#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <archive.h>
#include <archive_entry.h>
#include <brotli/decode.h>

int hydra_copydata(struct archive *ar, struct archive *aw);
bool hydra_extract(const char *file, const char *path);

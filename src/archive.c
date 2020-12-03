#include "archive.h"

int hydra_copydata(struct archive *ar, struct archive *aw) {
	int r;
	const void *buff;
	size_t size;
	int64_t off;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &off);
		if (r == ARCHIVE_EOF)
			return ARCHIVE_OK;
		if (r != ARCHIVE_OK)
			return r;

		r = archive_write_data_block(aw, buff, size, off);
		if (r != ARCHIVE_OK)
			return r;
	}
}

bool hydra_extract(const char *file, const char *path) {
	struct archive *a = archive_read_new();
	struct archive *ext = archive_write_disk_new();
	struct archive_entry *entry;
	int r, flags;

	flags =  ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME;
	flags += ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS | ARCHIVE_EXTRACT_XATTR;
	flags += ARCHIVE_EXTRACT_SECURE_SYMLINKS | ARCHIVE_EXTRACT_SECURE_NODOTDOT;

	archive_write_disk_set_options(ext, flags);
	archive_read_support_format_cpio(a);

	r = archive_read_open_filename(a, file, 16384);
	if (r != ARCHIVE_OK) {
		fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
		return false;
	}

	chdir(path);
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			return false;
		}

		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			return false;
		} else if (archive_entry_size(entry) > 0) {
			r = hydra_copydata(a, ext);
			if (r != ARCHIVE_OK) {
				fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
				return false;
			}
		}
		archive_write_finish_entry(ext);
	}

	archive_read_close(a);
	archive_read_free(a);    
	archive_write_close(ext);
	archive_write_free(ext);

	return true;
}

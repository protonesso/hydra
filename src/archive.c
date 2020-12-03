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

bool hydra_brotlidec(const char *file) {
	FILE *fp = fopen(file, "rb");
	if (!fp) {
		fprintf(stderr, "Failed to open file.\n");
		return false;
	}

	fseek(fp, 0 , SEEK_END);
	size_t fsize = ftell(fp);
	rewind(fp);

	char *buf = (char *)malloc(sizeof(char) * fsize);
	if (!buf) {
		fprintf(stderr, "Failed to allocate memory for file.\n");
		return false;
	}

	if (!(fread(buf, fsize, 1, fp))) {
		fprintf(stderr, "Failed to read file.\n");
		return false;
	}

	uint8_t outbuf[BUFSIZ];
	const void *inbuf = buf;
	size_t outbuf_size = sizeof(outbuf);
	size_t inbuf_size = sizeof(buf);

	BrotliDecoderState *state;
	BrotliDecoderResult result;
	size_t avail_in = inbuf_size;
	const uint8_t *next_in = inbuf;
	size_t avail_out = outbuf_size;
	uint8_t *next_out = outbuf;

	state = BrotliDecoderCreateInstance(NULL, NULL, NULL);
	if (!state) {
		fprintf(stderr, "Failed to create Brotli instance\n");
		return false;
	}

	result = BrotliDecoderDecompressStream(state,
				&avail_in, &next_in,
				&avail_out, &next_out,
				NULL);
	if (!result) {
		fprintf(stderr, "Failed to decode Brotli archive\n");
		return false;
	}

	switch (result) {
		case BROTLI_DECODER_RESULT_ERROR:
			printf("error\n");
			break;
		case BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT:
			printf("more input\n");
			break;
		case BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
			printf("more out\n");
			break;
		case BROTLI_DECODER_RESULT_SUCCESS:
			printf("success\n");
			break;
	}

	BrotliDecoderIsFinished(state);
	fclose(fp);
	free(buf);

	return true;
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
		fprintf(stderr, "Failed to extract archive: %s\n", archive_error_string(a));
		return false;
	}

	chdir(path);
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract archive: %s\n", archive_error_string(a));
			return false;
		}

		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract archive: %s\n", archive_error_string(a));
			return false;
		} else if (archive_entry_size(entry) > 0) {
			r = hydra_copydata(a, ext);
			if (r != ARCHIVE_OK) {
				fprintf(stderr, "Failed to extract archive: %s\n", archive_error_string(a));
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

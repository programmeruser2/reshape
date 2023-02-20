#include <stdio.h>
#include <stdlib.h>

FILE* fopen_with_error(char* path, char* mode) {
	FILE* f = fopen(path, mode);
	if (f == NULL) {
		printf("failed to open %s", path);
		perror("");
		exit(1);
	} else {
		return f;
	}
}
char* read_all(FILE* file) {
	// TODO: maybe check more file types?
	if (file == stdin) {
		char* buf = (char*) malloc(sizeof(char));
		size_t size = 1;
		if (buf == NULL) {
			perror("failed to allocate buffer for reading file");
			exit(1);
		}
		buf[0] = '\0';
		char c;
		while ((c = getchar() != EOF) {
			++size;
			char* newbuf = (char*) realloc(buf, size*(sizeof(char)));
			if (newbuf == NULL) {
				perror("realloc failed");
				free(newbuf);
				exit(1);
			} else {
				buf = newbuf;
				buf[size-1] = '\0';
				buf[size-2] = c;
			}
		}
		return buf;
	} else {
		if ((fseek(file, 0, SEEK_END)) < 0) {
			perror("seek failed");
			exit(1);
		}
		size_t size = ftell(file);
		rewind(file);
		char* buf = malloc(size+1);
		size_t bytesread;
		if ((bytesread = fread(buf, size, 1, file)) < size) {
			fprintf(stderr, "failed to read from file, only read %zu bytes\n", bytesread);
			free(buf);
			exit(1);
		}
		return buf;
	}
}

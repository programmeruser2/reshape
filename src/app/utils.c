#include <stdio.h>
#include <stdlib.h>

char* read_all(FILE* file) {
	// TODO: maybe check more file types?
	// TODO: handle null bytes correctly
	if (file == stdin) {
		char* buf = (char*) malloc(sizeof(char));
		size_t size = 1;
		if (buf == NULL) {
			return NULL;
		}
		buf[0] = '\0';
		int c;
		while ((unsigned int) (c = getchar()) != (unsigned int) EOF) {
			++size;
			char* newbuf = (char*) realloc(buf, size*(sizeof(char)));
			if (newbuf == NULL) {
				free(buf);
				return NULL;
			} else {
				buf = newbuf;
				buf[size-1] = '\0';
				buf[size-2] = (char) c;
			}
		}
		return buf;
	} else {
		if ((fseek(file, 0, SEEK_END)) < 0) {
			return NULL;
		}
		size_t size = (size_t) ftell(file);
		rewind(file);
		char* buf = malloc(size+1);
		if (fread(buf, size, 1, file) < size) {
			free(buf);
			return NULL;
		}
		buf[size] = '\0';
		return buf;
	}
}

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
FILE* fopen_with_error(char* path, char* mode);
char* read_all(FILE* file);

#endif

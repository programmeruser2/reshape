#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
// TODO: use something other than errno for errors

char** get_modules(char* config) {
	char** modules = NULL;
	// array of strings
	// {"mymodule", "/path/to/mymodule.so", ...}
	// TODO: use hash table O(1) instead of O(n) lookup array
	const char* delims = " \n\t";
	char* last = "";
	char* token = strtok(config, delims);
	size_t ntokens = 0;
	while (token != NULL) {
		last = token;
		++ntokens;

		char** newmodules = realloc(modules, ntokens * sizeof(char*));
		if (newmodules == NULL) {
			free(modules);
			return NULL; 
		} else {
			modules = newmodules;
			modules[ntokens-1] = token;
		}
		token = strtok(config, delims);
	}
	if (ntokens % 2 != 0) {
		errno = ELIBACC;
		free(modules);
		return NULL;
	}
	return modules;	
}

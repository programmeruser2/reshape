#include <string.h>
#include <stdlib.h>

char** get_modules(char* config) {
	char** modules = NULL;
	// array of strings
	// {"mymodule", "/path/to/mymodule.so", ...}
	const char* delims = " \n\t";
	char* last = "";
	char* token = strtok(config, delims);
	size_t ntokens = 0;
	while (token != NULL) {
		last = token;
		++ntokens;

		newmodules = realloc(modules, ntokens * sizeof(char*));
		if (newmodules == NULL) {
			perror("realloc failed");
			exit(1);
		} else {
			modules = newmodules;
			modules[ntokens-1] = token;
		}
		token = strtok(config, delims);
	}
	if (ntokens % 2 != 0) {
		fprintf(stderr, "Missing shared library path for module %s\n", last);
		exit(1);
	}
	return modules;	
}

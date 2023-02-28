#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char** module_convert(size_t nargs, char** args, char** error) {
	if (nargs < 1) {
		*error = "Please provide a number to be doubled.";
		return NULL;
	}

	char* end;
	errno = 0;
	long long n = strtoll(args[0], &end, 10);
	if (errno != 0) {
		*error = strerror(errno);
		return NULL;
	}

	long long doubled = 2*n;

	char** res = malloc(2*sizeof(char*));

	res[0] = malloc(21*sizeof(char));
	snprintf(res[0], 21, "%lld\n", doubled);

	res[1] = NULL;

	return res;
}

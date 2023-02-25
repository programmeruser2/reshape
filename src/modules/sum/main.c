#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

char** module_convert(size_t nargs, char** args, char** error) {
	long long sum = 0;
	bool flag = false;
	for (size_t i = 0; i < nargs; ++i) {
		char* end;
		char* s = args[i];
		errno = 0;
		bool notfirst = false;
		do {
			if (notfirst) {
				s = end;
			} else {
				notfirst = true;
			}
			long long n = strtoll(s, &end, 10);
			//printf("%lld", n);
			if (errno != 0) {
				*error = strerror(errno);
				flag = true;
				break;
			}
			sum += n;
		} while (s != end);
		if (flag) break;
	}
	if (flag) return NULL;
	char** res = malloc(2*sizeof(char*));
	res[0] = malloc(21*sizeof(char));
	snprintf(res[0], 21, "%lld\n", sum);
	res[1] = NULL;
	return res;
}

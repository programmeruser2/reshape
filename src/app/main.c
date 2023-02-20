#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int c;
	while ((c = getopt(argc, argv, "ht:")) != -1) {
		switch (c) {
			case 'h':
				printf("Usage: %s [-t type1 type2 type3...]\n", argv[0]);
				break;
			case 't':
				break;
			case '?':
				// fprintf(stderr, "Unknown option -%c", optopt);
				// by default, opterr =/= 0 so error messages are automatically printed
				// uncomment and set opterr = 0 for a custom error message
				return 1;
			default:
				fprintf(stderr, "Internal error (unknown getopt return value)");
				return 1;
		}
	}
}

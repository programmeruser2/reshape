#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int c;
	while ((c = getopt(argc, argv, "ht:")) != -1) {
		switch (c) {
			case 'h':
				printf("Usage: %s -t type1 [-t type2...] [-i input1] [-i input2...] [-o output1] [-o output2...]\n"
							"-h			Show this help message\n"
							"-i input			specify an input file (input files that appear earlier in the arguments are earlier in the list of inputs\n"
							"-o output			specify an output file (output files that appear earlier in the arguments are earlier in the list of outputs)\n", argv[0]);
				return 0;
			case 't':
				// Read in conversion types.
				// If there are multiple, then the output of one is piped into the next.

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

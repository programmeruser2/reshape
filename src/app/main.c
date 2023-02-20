#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

int main(int argc, char* argv[]) {
	int c;
	
	// Arrays for storing types, inputs, and outputs

	char** types = NULL;
	size_t ntypes = 0;
	
	char** inputs = NULL;
	size_t ninputs = 0;

	char** outputs = NULL;
	size_t noutputs = 0;

	while (optind < argc) {
		if ((c = getopt(argc, argv, "ht:o:")) != -1) {
			switch (c) {
				case 'h':
					printf("Usage: %s -t type1 [-t type2...] [-o output1] [-o output2...] input1 [input2...]\n"
							"-h		Show this help message\n"
							"-t		Specify conversion type (if multiple are provided, outputs are piped into inputs in the order that the conversion types are specified)\n"
							"-o output		specify an output file (output files that appear earlier in the arguments are earlier in the list of outputs)\n", argv[0]);
					return 0;
				case 't':
					// Read in conversion types.
					// If there are multiple, then the output of one is piped into the next
					++ntypes;
					types = (char**) realloc(types, ntypes * sizeof(char*));
					types[ntypes-1] = optarg;
					break;
				case 'o':
					++noutputs;
					outputs = (char**) realloc(outputs, noutputs * sizeof(char*));
					outputs[noutputs-1] = optarg;
				case '?':
					// fprintf(stderr, "Unknown option -%c", optopt);
					// by default, opterr =/= 0 so error messages are automatically printed
					// uncomment and set opterr = 0 for a custom error message
					return 1;
				default:
					fprintf(stderr, "Internal error (unknown getopt return value)");
					return 1;
			}
		} else {
			// process input
			++ninputs;
			inputs = (char**) realloc(inputs, ninputs * sizeof(char*));
			inputs[ninputs-1] = argv[optind];
			++optind;
		}
	}

	// Free memory
	free(types);
	free(inputs);
	free(outputs);
	return 0;
}

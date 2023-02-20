#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include "utils.h"
#include "config.h"

// TODO: better memory exit handler

int main(int argc, char* argv[]) {
	int c;
	
	// Arrays for storing types, inputs, and outputs

	char** types = NULL;
	size_t ntypes = 0;
	
	FILE** inputs = NULL;
	size_t ninputs = 0;

	FILE** outputs = NULL;
	size_t noutputs = 0;

	char* configpath = "/etc/reshape.conf";

	while (optind < argc) {
		if ((c = getopt(argc, argv, "ht:o:")) != -1) {
			switch (c) {
				case 'h':
					printf("Usage: %s -t type1 [-t type2...] [-o output1] [-o output2...] [-c configfile] input1 [input2...]\n"
							"default input: stdin\n"
							"-h		Show this help message\n"
							"-t		Specify conversion type (if multiple are provided, outputs are piped into inputs in the order that the conversion types are specified)\n"
							"-o output		specify an output file (default: stdout, output files that appear earlier in the arguments are earlier in the list of outputs)\n"
							"-c configfile		specify a config file (default: /etc/reshape.conf)\n", argv[0]);
					return 0;
				case 't':
					// Read in conversion types.
					// If there are multiple, then the output of one is piped into the next
					++ntypes;
					char** newtypes = (char**) realloc(types, ntypes * sizeof(char*));
					if (newtypes == NULL) {
						perror("realloc types failed");
						free(types);
						return 1;
					}
					types = newtypes;
					types[ntypes-1] = optarg;
					break;
				case 'o':
					++noutputs;
					FILE** newoutputs = (FILE**) realloc(outputs, noutputs * sizeof(FILE*));
					if (newoutputs == NULL) {
						perror("realloc outputs failed");
						free(outputs);
						return 1;
					}
					outputs = newoutputs;
					outputs[noutputs-1] = fopen_with_error(optarg, "r");
					break;
				case 'c':
					configpath = optarg;
					break;
				case '?':
					// fprintf(stderr, "Unknown option -%c", optopt);
					// by default, opterr =/= 0 so error messages are automatically printed
					// uncomment and set opterr = 0 for a custom error message
					return 1;
				default:
					fprintf(stderr, "Internal error (unknown getopt return value)\n");
					return 1;
			}
		} else {
			// process input
			++ninputs;
			FILE** newinputs = (FILE**) realloc(outputs, noutputs * sizeof(FILE*));
			if (newinputs == NULL) {
				perror("realloc inputs failed");
				free(inputs);
				return 1;
			}
			inputs = newinputs;
			inputs[ninputs-1] = fopen_with_error(argv[optind], "r");
			++optind;
		}
	}
	// Handle required inputs
	if (ntypes == 0) {
		fprintf(stderr, "error: no conversion types specified\n");
		return 1;
	}
	if (noutputs == 0) {
		noutputs = 1;
		outputs[0] = stdout; // stdout
	}

	if (ninputs == 0) {
		ninputs = 1;
		inputs[0] = stdin;
	}
	// Open config file
	FILE* configfile = fopen_with_error(configpath, "r");
	// Read and parse

	// Conversion

	// Close all fil
	for (size_t i = 0; i < noutputs; ++i) {
		fclose(outputs[i]);
	}
	for (size_t i = 0; i < ninputs; ++i) {
		fclose(inputs[i]);
	}
	fclose(configfile);

	// Free memory
	
	free(types);
	free(inputs);
	free(outputs);
	return 0;
}

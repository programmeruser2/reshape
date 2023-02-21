#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include "utils.h"
#include "config.h"

// TODO: better memory exit handler

// optionslist
struct optionslist {
	char** types;
	size_t ntypes;

	FILE** inputs;
	size_t ninputs;

	FILE** outputs;
	size_t noutputs;

	FILE* configfile;
};

struct optionslist* create_optionslist(void) {
	struct optionslist* options = (struct optionslist*) malloc(sizeof(struct optionslist));

	options->types = NULL;
	options->ntypes = 0;

	options->inputs = NULL;
	options->ninputs = 0;

	options->outputs = NULL;
	options->noutputs = 0;

	options->configfile = NULL;

	return options;
}
void destroy_optionslist(struct optionslist* options) {
	// types should point to argv, so no need to dealloc individual pointer
	free(options->types);
	// close all files
	for (size_t i = 0; i < (options->ninputs); ++i) {
		if (options->inputs[i] != NULL) fclose(options->inputs[i]);
	}
	for (size_t i = 0; i < (options->noutputs); ++i) {
		if (options->outputs[i] != NULL) fclose(options->outputs[i]);
	}
	if (options->configfile != NULL) fclose(options->configfile);
	// finally, free the entire struct
	free(options);
}

int main(int argc, char* argv[]) {
	int c;
	
	struct optionslist* options = create_optionslist();

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
					destroy_optionslist(options);
					return 0;
				case 't':
					// Read in conversion types.
					// If there are multiple, then the output of one is piped into the next
					++options->ntypes;
					char** newtypes = (char**) realloc(options->types, (options->ntypes) * sizeof(char*));
					if (newtypes == NULL) {
						perror("realloc types failed");
						destroy_optionslist(options);
						return 1;
					}
					options->types = newtypes;
					options->types[options->ntypes-1] = optarg;
					break;
				case 'o':
					++options->noutputs;
					FILE** newoutputs = (FILE**) realloc(options->outputs, (options->noutputs) * sizeof(FILE*));
					if (newoutputs == NULL) {
						perror("realloc outputs failed");
						destroy_optionslist(options);
						return 1;
					}
					options->outputs = newoutputs;
					options->outputs[(options->noutputs)-1] = fopen(optarg, "r");
					if (options->outputs[(options->noutputs)-1] == NULL) {
						perror("opening output file failed");
						destroy_optionslist(options);
						return 1;
					}
					break;
				case 'c':
					configpath = optarg;
					break;
				case '?':
					// fprintf(stderr, "Unknown option -%c", optopt);
					// by default, opterr =/= 0 so error messages are automatically printed
					// uncomment and set opterr = 0 for a custom error message
					destroy_optionslist(options);
					return 1;
				default:
					fprintf(stderr, "Internal error (unknown getopt return value)\n");
					destroy_optionslist(options);
					return 1;
			}
		} else {
			// process input
			++options->ninputs;
			FILE** newinputs = (FILE**) realloc(options->inputs, (options->ninputs) * sizeof(FILE*));
			if (newinputs == NULL) {
				perror("realloc inputs failed");
				destroy_optionslist(options);
				return 1;
			}
			options->inputs = newinputs;
			options->inputs[(options->ninputs)-1] = fopen(argv[optind], "r");
			if (options->inputs[(options->ninputs)-1] == NULL) {
				perror("opening input file failed");
				destroy_optionslist(options);
				return 1;
			}
			++optind;
		}
	}
	// Handle required inputs
	if (options->ntypes == 0) {
		fprintf(stderr, "error: no conversion types specified\n");
		destroy_optionslist(options);
		return 1;
	}
	if (options->noutputs == 0) {
		options->noutputs = 1;
		options->outputs[0] = stdout; // stdout
	}

	if (options->ninputs == 0) {
		options->ninputs = 1;
		options->inputs[0] = stdin;
	}
	// Open config file
	options->configfile = fopen(configpath, "r");
	if (options->configfile == NULL) {
		perror("failed to open config file");
		destroy_optionslist(options);
		return 1;
	}
	// Read and parse

	// Conversion

	// Cleanup
	destroy_optionslist(options);
	return 0;
}

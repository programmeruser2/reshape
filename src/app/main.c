#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>

// TODO: be consistent with perror/strerror

#include "utils.h"
#include "config.h"

// optionslist
struct optionslist {
	char** types;
	size_t ntypes;

	FILE** inputs;
	char** inputbufs;
	size_t ninputs;

	FILE** outputs;
	size_t noutputs;

	FILE* configfile;
	
	char** modules;
	void** modulehandles;
};

struct optionslist* create_optionslist(void) {
	struct optionslist* options = (struct optionslist*) malloc(sizeof(struct optionslist));

	options->types = NULL;
	options->ntypes = 0;

	options->inputs = NULL;
	options->inputbufs = NULL;
	options->ninputs = 0;

	options->outputs = NULL;
	options->noutputs = 0;

	options->configfile = NULL;

	options->modules = NULL;
	options->modulehandles = NULL;

	return options;
}
void destroy_optionslist(struct optionslist* options) {
	// types should point to argv, so no need to dealloc individual pointer
	// close all files
	for (size_t i = 0; i < (options->ninputs); ++i) {
		if (options->inputs != NULL && options->inputs[i] != NULL) fclose(options->inputs[i]);
		if (options->inputbufs != NULL && options->inputbufs[i] != NULL) free(options->inputbufs[i]);
	}
	for (size_t i = 0; i < (options->noutputs); ++i) {
		if (options->outputs != NULL && options->outputs[i] != NULL) fclose(options->outputs[i]);
	}
	for (size_t i = 0; i < (options->ntypes); ++i) {
		if (options->modulehandles != NULL && options->modulehandles[i] != NULL) dlclose(options->modulehandles[i]);
	}
	if (options->configfile != NULL) fclose(options->configfile);
	// free individual arrays
	free(options->types);
	free(options->inputs);
	free(options->inputbufs);
	free(options->outputs);
	free(options->modules);
	free(options->modulehandles);
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
					options->outputs[(options->noutputs)-1] = fopen(optarg, "w+");
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
			char** newinputbufs = (char**) realloc(options->inputbufs, (options->ninputs) * sizeof(char*));
			if (newinputs == NULL) {
				perror("realloc inputs failed");
				destroy_optionslist(options);
				return 1;
			}
			if (newinputbufs == NULL) {
				perror("realloc inputbufs failed");
				destroy_optionslist(options);
				return 1;
			}
			options->inputs = newinputs;
			options->inputbufs = newinputbufs;
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
	// TODO: util for specifying # of inputs/outputs for a module
	if (options->noutputs == 0) {
		options->noutputs = 1;
		options->outputs = malloc(sizeof(FILE*));
		if (options->outputs == NULL) {
			perror("malloc for outputs failed");
			destroy_optionslist(options);
			return 1;
		}
		options->outputs[0] = stdout; // stdout
	}

	if (options->ninputs == 0) {
		options->ninputs = 1;
		options->inputs = malloc(sizeof(FILE*));
		if (options->inputs == NULL) {
			perror("malloc for inputs failed");
			destroy_optionslist(options);
			return 1;
		}
		options->inputbufs = malloc(sizeof(char*));
		if (options->inputbufs == NULL) {
			perror("malloc for inputbufs failed");
			destroy_optionslist(options);
			return 1; 
		}
		options->inputs[0] = stdin;
	}
	// Open config file
	options->configfile = fopen(configpath, "r");
	if (options->configfile == NULL) {
		perror("failed to open config file");
		destroy_optionslist(options);
		return 1;
	}
	// Read and parse config
	// TODO: allow config file specfication in enviroment variables
	char* config = read_all(options->configfile);
	if (config == NULL) {
		perror("failed to read configuration file");
		destroy_optionslist(options);
		return 1;
	}
	options->modules = get_modules(config);
	// no need for config now
	free(config);	
	if (options->modules == NULL) {
		perror("failed to parse configuration file");
		destroy_optionslist(options);
		return 1;
	}
	
	//  Read all input files
	for (size_t i = 0; i < options->ninputs; ++i) {
		options->inputbufs[i] = read_all(options->inputs[i]);
		if (options->inputbufs[i] == NULL) {
			// TODO: way to get file path from FILE pointer?
			perror("failed to read input file");
			destroy_optionslist(options);
			return 1;
		}
	}

	// Conversion
	// Allocate space for module handles
	options->modulehandles = malloc((options->ntypes) * sizeof(void*));
	if (options->modulehandles == NULL) {
		perror("Failed to allocate space for shared library handles for modules");
		destroy_optionslist(options);
		return 1;
	}
	// First, find the modules we need
	// Loop through inputted types
	for (size_t i = 0; i < options->ntypes; ++i) {
		// Search for library path in options->modules
		// TODO: handle symbol conflicts better
		// TODO: see https://stackoverflow.com/questions/22004131/is-there-symbol-conflict-when-loading-two-shared-libraries-with-a-same-symbol
		size_t j = 0;
		bool found = false;
		while (options->modules[j] != NULL) {
			if (options->modules[j] == options->types[i]) {
				options->modulehandles[i] = dlopen(options->modules[j+1], RTLD_LAZY | RTLD_DEEPBIND);
				// RTLD_LAZY: we don't want to load more functions than needed
				// RTLD_DEEPBIND: make sure that we are actually loading the conversion function from the library,
				// and not some other library with a global symbol
				if (options->modulehandles[i] == NULL) {
					printf("Failed to load shared library %s for module %s: %s", options->modules[j+1], options->modules[j], dlerror());
					destroy_optionslist(options);
					return 1;
				} else {
					found = true;
					break;
				}
			}
			j += 2;
		}
		if (!found) {
			fprintf(
				stderr, 
				"Could not find shared library path for module %s in configuration file %s\n", 
				options->types[i], 
				configpath
			);
			destroy_optionslist(options);
			return 1;
		}
	}
	
	// Now, start conversion!
	// Go through every type
	// Earlier types -> earlier in piping/composition
	char** current_args = options->inputbufs;
	size_t current_nargs = options->ninputs;
	// TODO: for stdin/stdout defaults, maybe "pad" with more stdin/stdout to make everything go to stdin/stdout?
	for (size_t i = 0; i < options->ntypes; ++i) {
		// Get reference of function
		// Function prototype
		char** (*module_convert) (size_t, char**, char**);
		// Inputs are passed through to be char**.
		// Since va_list is not supported in many languages and unsafe, a char** is used;
		// # of inputs are provided as a size_t
		// Full prototype: char** module_convert(size_t nargs, char** args, char** error);
		// error is a pointer to a error string. Printed if it is not equal to NULL and length > 0. 
		// The module SHOULD NOT assume anything about the error string.

		// this trick is necessary to prevent compiler errors/warnings about function pointers and void*
		*(void**) (&module_convert) = dlsym(options->modulehandles[i], "module_convert");
		char* error = NULL;
		char** output = module_convert(current_nargs, current_args, &error);
		// Check error
		if (error != NULL && strlen(error) > 0) {
			fprintf(stderr, "Error during conversion in module %s: %s\n", options->types[i], error);
			destroy_optionslist(options);
			return 1;
		}
		
		// Free previous memory
		// It's assumed that the strings are heap allocated. If it isn't the module can just malloc and then strcpy there.
		// The list should be terminated by a NULL.
		for (size_t j = 0; j < current_nargs; ++j) {
			free(current_args[i]);		
		}
		free(current_args);
		// Set new values
		current_args = output;
		current_nargs = 0;
		while (current_args[current_nargs] != NULL) ++current_nargs;
	}
	// Finally, write output to files
	for (size_t i = 0; i < current_nargs; ++i) {
		if (i < options->noutputs) {
			char* buf = current_args[i];
			size_t len = strlen(buf);
			fwrite(buf, len, 1, options->outputs[i]);
		} else {
			break;
		}
	}

	// We are done!

	// Cleanup
	destroy_optionslist(options);
	return 0;
}

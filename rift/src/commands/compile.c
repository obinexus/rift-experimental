#include "rift/cli/commands.h"
#include "rift/rift.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * rift_cli_cmd_compile - entry point for the `compile` command.
 * Parses simple command line options and executes the full
 * compilation pipeline using rift_compile().
 */
int rift_cli_cmd_compile(int argc, char *argv[]) {
    const char *input_file = NULL;
    const char *output_file = NULL;
    const char *config_file = NULL;
    bool verbose = false;

    static struct option long_opts[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"config", required_argument, 0, 'c'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    /* reset getopt for repeated use */
    optind = 1;
    int opt;
    while ((opt = getopt_long(argc, argv, "i:o:c:vhV", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
                printf("Usage: rift compile -i <input> -o <output> [options]\n");
                printf("  -c, --config FILE  Use configuration file\n");
                printf("  -v, --verbose      Verbose output\n");
                printf("  -h, --help         Show this help message\n");
                printf("  -V, --version      Show compiler version\n");
                return RIFT_SUCCESS;
            case 'V':
                printf("RIFT Compiler v%s\n", RIFT_VERSION_STRING);
                return RIFT_SUCCESS;
            default:
                fprintf(stderr, "Unknown option. Use --help for usage.\n");
                return RIFT_ERROR_INVALID_ARGUMENT;
        }
    }

    if (!input_file || !output_file) {
        fprintf(stderr, "compile: input and output files are required\n");
        return RIFT_ERROR_INVALID_ARGUMENT;
    }

    rift_context_t *ctx = rift_init(config_file);
    if (!ctx) {
        fprintf(stderr, "Failed to initialize compiler context\n");
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }

    int result = rift_compile(ctx, input_file, output_file);

    if (verbose) {
        if (result == RIFT_SUCCESS) {
            printf("Compilation succeeded\n");
        } else {
            printf("Compilation failed with code %d\n", result);
        }
    }

    rift_cleanup(ctx);
    return result;
}


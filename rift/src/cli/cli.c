/*
 * cli.c - RIFT Command Line Interface Implementation
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#include "rift/cli/cli.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>

int rift_cli_parse_args(int argc, char **argv, rift_cli_options_t *options) {
    memset(options, 0, sizeof(rift_cli_options_t));
    
    // Set defaults
    options->threads = 32;
    options->architecture = strdup("amd_ryzen");
    options->config_file = strdup(".riftrc");
    
    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"config", required_argument, 0, 'c'},
        {"verbose", no_argument, 0, 'v'},
        {"debug", no_argument, 0, 'd'},
        {"bottom-up", no_argument, 0, 'b'},
        {"top-down", no_argument, 0, 't'},
        {"threads", required_argument, 0, 'j'},
        {"architecture", required_argument, 0, 'a'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };
    
    int c;
    while ((c = getopt_long(argc, argv, "i:o:c:vdbtj:a:hV", long_options, NULL)) != -1) {
        switch (c) {
            case 'i':
                options->input_file = strdup(optarg);
                break;
            case 'o':
                options->output_file = strdup(optarg);
                break;
            case 'c':
                free(options->config_file);
                options->config_file = strdup(optarg);
                break;
            case 'v':
                options->verbose = true;
                break;
            case 'd':
                options->debug = true;
                break;
            case 'b':
                options->bottom_up = true;
                break;
            case 't':
                options->top_down = true;
                break;
            case 'j':
                options->threads = atoi(optarg);
                break;
            case 'a':
                free(options->architecture);
                options->architecture = strdup(optarg);
                break;
            case 'h':
                rift_cli_print_help();
                return 1;
            case 'V':
                rift_cli_print_version();
                return 1;
            default:
                return -1;
        }
    }
    
    return 0;
}

int rift_cli_execute(rift_cli_options_t *options) {
    if (!options->input_file) {
        fprintf(stderr, "Error: Input file required\n");
        return -1;
    }
    
    if (!options->output_file) {
        fprintf(stderr, "Error: Output file required\n");
        return -1;
    }
    
    rift_context_t *ctx = rift_init(options->config_file);
    if (!ctx) {
        fprintf(stderr, "Error: Failed to initialize RIFT context\n");
        return -1;
    }
    
    ctx->debug_enabled = options->debug;
    ctx->thread_count = options->threads;
    
    int result = rift_compile(ctx, options->input_file, options->output_file);
    
    rift_cleanup(ctx);
    return result;
}

void rift_cli_print_help(void) {
    printf("RIFT Compiler v%s\n", RIFT_VERSION_STRING);
    printf("Usage: rift [OPTIONS]\n\n");
    printf("Options:\n");
    printf("  -i, --input FILE       Input .rift source file\n");
    printf("  -o, --output FILE      Output bytecode file (.rbc)\n");
    printf("  -c, --config FILE      Configuration file (default: .riftrc)\n");
    printf("  -v, --verbose          Verbose output\n");
    printf("  -d, --debug            Enable debug mode\n");
    printf("  -b, --bottom-up        Enable bottom-up parsing\n");
    printf("  -t, --top-down         Enable top-down parsing\n");
    printf("  -j, --threads N        Number of threads (default: 32)\n");
    printf("  -a, --architecture A   Target architecture (default: amd_ryzen)\n");
    printf("  -h, --help             Show this help message\n");
    printf("  -V, --version          Show version information\n");
    printf("\nOBINexus Computing Framework - Waterfall Methodology\n");
}

void rift_cli_print_version(void) {
    printf("RIFT Compiler v%s\n", RIFT_VERSION_STRING);
    printf("OBINexus Computing Framework\n");
    printf("Technical Lead: Nnamdi Okpala\n");
    printf("Built: %s %s\n", __DATE__, __TIME__);
}

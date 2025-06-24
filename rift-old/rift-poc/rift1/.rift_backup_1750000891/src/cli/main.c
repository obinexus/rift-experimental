// RIFT Stage 1 CLI Implementation
// OBINexus Computing - AEGIS Framework

#include "../../include/rift1/rift.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CLI command structure
typedef struct {
    char* input_file;
    char* output_file;
    char* config_file;
    bool verbose;
    bool debug;
    RiftConfig* rift_config;
    RiftEngine* engine;
    RiftAutomaton* automaton;
} CliOptions;

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS] INPUT_FILE\n", program_name);
    printf("\nOBINexus RIFT Stage 1 - syntax_analysis\n");
    printf("\nOptions:\n");
    printf("  -o, --output FILE     Output file (default: INPUT.rift.1)\n");
    printf("  -c, --config FILE     Configuration file\n");
    printf("  -v, --verbose         Enable verbose output\n");
    printf("  -d, --debug           Enable debug mode\n");
    printf("  -h, --help            Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s input.rift.0\n", program_name);
    printf("  %s -v -o output.rift.1 input.rift.0\n", program_name);
}

int main(int argc, char* argv[]) {
    CliOptions opts = {0};
    
    static struct option long_options[] = {
        {"output",  required_argument, 0, 'o'},
        {"config",  required_argument, 0, 'c'},
        {"verbose", no_argument,       0, 'v'},
        {"debug",   no_argument,       0, 'd'},
        {"help",    no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "o:c:vdh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'o':
                opts.output_file = optarg;
                break;
            case 'c':
                opts.config_file = optarg;
                break;
            case 'v':
                opts.verbose = true;
                break;
            case 'd':
                opts.debug = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                print_usage(argv[0]);
                return 1;
            default:
                abort();
        }
    }
    
    // Validate arguments
    if (optind >= argc) {
        fprintf(stderr, "Error: Input file required\n");
        print_usage(argv[0]);
        return 1;
    // Generate default output file if not specified
    if (!opts.output_file) {
        size_t input_len = strlen(opts.input_file);
        opts.output_file = malloc(input_len + 8); // +8 for ".rift.1\0"
        if (!opts.output_file) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return 1;
        }
        strncpy(opts.output_file, opts.input_file, input_len);
        strcat(opts.output_file, ".rift.1");
    }
    
    // Initialize RIFT components
    opts.rift_config = rift_config_create();
    if (!opts.rift_config) {
        fprintf(stderr, "Error: Failed to create RIFT configuration\n");
        return 1;
    }
    
    opts.automaton = rift_automaton_create();
    if (!opts.automaton) {
        fprintf(stderr, "Error: Failed to create RIFT automaton\n");
        rift_config_destroy(opts.rift_config);
        return 1;
    }
    
    opts.engine = rift_engine_create();
    if (!opts.engine) {
        fprintf(stderr, "Error: Failed to create RIFT engine\n");
        rift_automaton_destroy(opts.automaton);
        rift_config_destroy(opts.rift_config);
        return 1;
    }
    
    opts.engine->automaton = opts.automaton;
    
    if (opts.verbose) {
        printf("🚀 RIFT Stage 1 Processing\n");
        printf("Input: %s\n", opts.input_file);
        printf("Output: %s\n", opts.output_file);
    }
    
    // Process the input file
    RiftResult result = rift_process_file(opts.input_file, opts.output_file, opts.rift_config);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "Error: Processing failed - %s\n", rift_result_string(result));
        rift_engine_destroy(opts.engine);
        rift_automaton_destroy(opts.automaton);
        rift_config_destroy(opts.rift_config);
        return 1;
    }
    
    if (opts.verbose) {
        printf("✅ Stage 1 processing complete\n");
    }
    
    // Cleanup
    rift_engine_destroy(opts.engine);
    rift_automaton_destroy(opts.automaton);
    rift_config_destroy(opts.rift_config);
    if (opts.output_file != opts.input_file) {
        free(opts.output_file);
    }
    
    return 0;
}
    printf("✅ Stage 1 processing complete\n");
    
    return 0;
}

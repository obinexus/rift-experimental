#include "../include/rift0/rift.h"
#include <getopt.h>

static struct option long_options[] = {
    {"output",  required_argument, 0, 'o'},
    {"config",  required_argument, 0, 'c'},
    {"verbose", no_argument,       0, 'v'},
    {"debug",   no_argument,       0, 'd'},
    {"help",    no_argument,       0, 'h'},
    {"version", no_argument,       0, 'V'},
    {0, 0, 0, 0}
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        rift_print_usage(argv[0]);
        return 1;
    }
    
    // Default configuration
    RiftConfig* config = rift_config_create();
    if (!config) {
        fprintf(stderr, "Error: Failed to create configuration\n");
        return 1;
    }
    
    char* input_file = NULL;
    char* output_dir = NULL;
    char* config_file = ".riftrc";
    
    // Parse command line arguments
    int c;
    int option_index = 0;
    
    while ((c = getopt_long(argc, argv, "o:c:vdhV", long_options, &option_index)) != -1) {
        switch (c) {
            case 'o':
                output_dir = optarg;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'v':
                config->verbose = true;
                break;
            case 'd':
                config->debug_mode = true;
                break;
            case 'h':
                rift_print_usage(argv[0]);
                rift_config_destroy(config);
                return 0;
            case 'V':
                rift_print_version();
                rift_config_destroy(config);
                return 0;
            case '?':
                rift_print_usage(argv[0]);
                rift_config_destroy(config);
                return 1;
            default:
                break;
        }
    }
    
    // Get input file from remaining arguments
    if (optind < argc) {
        input_file = argv[optind];
    } else {
        fprintf(stderr, "Error: No input file specified\n");
        rift_print_usage(argv[0]);
        rift_config_destroy(config);
        return 1;
    }
    
    // Load configuration file if it exists
    if (rift_config_load(config, config_file) == RIFT_SUCCESS && config->verbose) {
        printf("Loaded configuration from '%s'\n", config_file);
    }
    
    // Override output directory if specified
    if (output_dir) {
        free(config->output_dir);
        config->output_dir = strdup(output_dir);
    }
    
    // Generate output filename
    char output_file[512];
    char* base_name = strrchr(input_file, '/');
    base_name = base_name ? base_name + 1 : input_file;
    
    char* dot = strrchr(base_name, '.');
    if (dot) {
        size_t name_len = (size_t)(dot - base_name);
        snprintf(output_file, sizeof(output_file), "%s/%.*s.rift.0", 
                config->output_dir, (int)name_len, base_name);
    } else {
        snprintf(output_file, sizeof(output_file), "%s/%s.rift.0", 
                config->output_dir, base_name);
    }
    
    if (config->verbose) {
        printf("RIFT Stage 0 - Tokenization/Lexical Analysis\n");
        printf("Input:  %s\n", input_file);
        printf("Output: %s\n", output_file);
        printf("Config: %s\n", config_file);
    }
    
    // Create output directory if it doesn't exist
    char mkdir_cmd[600];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", config->output_dir);
    if (system(mkdir_cmd) != 0 && config->verbose) {
        printf("Warning: Could not create output directory\n");
    }
    
    // Process the file
    RiftResult result = rift_process_file(input_file, output_file, config);
    
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "Error: %s\n", rift_result_string(result));
        rift_config_destroy(config);
        return 1;
    }
    
    if (config->verbose) {
        printf("✅ Stage 0 processing complete\n");
        printf("💡 Next: Stage 1 (Parsing/Grammar Analysis)\n");
    }
    
    rift_config_destroy(config);
    return 0;
}

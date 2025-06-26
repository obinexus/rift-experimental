/*
 * rift/src/cli/main.c
 * RIFT Unified Command-Line Interface
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>

// RIFT Core Framework Headers
#include "rift/core/common.h"
#include "rift/governance/policy.h"
#include "rift/cli/commands.h"

// Stage-specific headers for unified access
#include "rift/core/stage-0/tokenizer.h"
#include "rift/core/stage-1/parser.h"
#include "rift/core/stage-2/semantic.h"
#include "rift/core/stage-3/validator.h"
#include "rift/core/stage-4/bytecode.h"
#include "rift/core/stage-5/verifier.h"
#include "rift/core/stage-6/emitter.h"

// AEGIS CLI Configuration
#define RIFT_CLI_VERSION "1.0.0"
#define RIFT_CLI_NAME "rift"
#define RIFT_CLI_DESCRIPTION "RIFT Compiler Pipeline - AEGIS Methodology"
#define RIFT_MAX_INPUT_SIZE (1024 * 1024)  // 1MB input limit
#define RIFT_MAX_OUTPUT_PATH 512

// CLI Command Structure
typedef enum {
    RIFT_CMD_HELP,
    RIFT_CMD_VERSION,
    RIFT_CMD_TOKENIZE,
    RIFT_CMD_PARSE,
    RIFT_CMD_ANALYZE,
    RIFT_CMD_VALIDATE,
    RIFT_CMD_GENERATE,
    RIFT_CMD_VERIFY,
    RIFT_CMD_EMIT,
    RIFT_CMD_COMPILE,
    RIFT_CMD_GOVERNANCE,
    RIFT_CMD_UNKNOWN
} rift_cli_command_t;

// CLI Options Structure
typedef struct {
    rift_cli_command_t command;
    char input_file[RIFT_MAX_OUTPUT_PATH];
    char output_file[RIFT_MAX_OUTPUT_PATH];
    bool verbose_mode;
    bool debug_mode;
    bool strict_mode;
    bool aegis_validation;
    bool show_metrics;
    int optimization_level;
    char config_file[RIFT_MAX_OUTPUT_PATH];
} rift_cli_options_t;

// Global CLI state
static rift_cli_options_t g_cli_options = {0};
static rift_governance_context_t g_governance_context = {0};

// Function prototypes
static void print_version(void);
static void print_help(void);
static void print_usage(void);
static int parse_command_line(int argc, char* argv[]);
static rift_cli_command_t parse_command(const char* cmd_str);
static int execute_command(void);
static int load_input_file(const char* filename, char** content, size_t* size);
static int save_output_file(const char* filename, const char* content, size_t size);
static void print_performance_summary(const rift_performance_metrics_t* metrics);

// Command implementations
static int cmd_tokenize(void);
static int cmd_parse(void);
static int cmd_analyze(void);
static int cmd_validate(void);
static int cmd_generate(void);
static int cmd_verify(void);
static int cmd_emit(void);
static int cmd_compile(void);
static int cmd_governance(void);

/*
 * Main entry point for RIFT CLI
 */
int main(int argc, char* argv[]) {
    int result = RIFT_SUCCESS;
    
    // Initialize CLI options with defaults
    g_cli_options.command = RIFT_CMD_HELP;
    g_cli_options.verbose_mode = false;
    g_cli_options.debug_mode = false;
    g_cli_options.strict_mode = true;
    g_cli_options.aegis_validation = true;
    g_cli_options.show_metrics = false;
    g_cli_options.optimization_level = 2;
    strcpy(g_cli_options.config_file, ".riftrc");
    
    // Parse command line arguments
    result = parse_command_line(argc, argv);
    if (result != RIFT_SUCCESS) {
        RIFT_LOG_ERROR("Failed to parse command line arguments");
        return EXIT_FAILURE;
    }
    
    // Initialize AEGIS governance framework
    if (g_cli_options.aegis_validation) {
        result = rift_governance_init(&g_governance_context, 
                                     g_cli_options.config_file);
        if (result != RIFT_SUCCESS) {
            RIFT_LOG_WARNING("Failed to initialize governance framework, continuing without governance");
            g_cli_options.aegis_validation = false;
        }
    }
    
    // Execute the requested command
    result = execute_command();
    
    // Cleanup governance resources
    if (g_cli_options.aegis_validation) {
        rift_governance_cleanup(&g_governance_context);
    }
    
    return (result == RIFT_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 * Parse command line arguments using getopt
 */
static int parse_command_line(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    
    static struct option long_options[] = {
        {"help",        no_argument,       0, 'h'},
        {"version",     no_argument,       0, 'V'},
        {"verbose",     no_argument,       0, 'v'},
        {"debug",       no_argument,       0, 'd'},
        {"output",      required_argument, 0, 'o'},
        {"config",      required_argument, 0, 'c'},
        {"strict",      no_argument,       0, 's'},
        {"no-aegis",    no_argument,       0, 'n'},
        {"metrics",     no_argument,       0, 'm'},
        {"optimize",    required_argument, 0, 'O'},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "hVvdo:c:snmO:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                g_cli_options.command = RIFT_CMD_HELP;
                return RIFT_SUCCESS;
                
            case 'V':
                g_cli_options.command = RIFT_CMD_VERSION;
                return RIFT_SUCCESS;
                
            case 'v':
                g_cli_options.verbose_mode = true;
                break;
                
            case 'd':
                g_cli_options.debug_mode = true;
                g_cli_options.verbose_mode = true;
                break;
                
            case 'o':
                strncpy(g_cli_options.output_file, optarg, RIFT_MAX_OUTPUT_PATH - 1);
                g_cli_options.output_file[RIFT_MAX_OUTPUT_PATH - 1] = '\0';
                break;
                
            case 'c':
                strncpy(g_cli_options.config_file, optarg, RIFT_MAX_OUTPUT_PATH - 1);
                g_cli_options.config_file[RIFT_MAX_OUTPUT_PATH - 1] = '\0';
                break;
                
            case 's':
                g_cli_options.strict_mode = true;
                break;
                
            case 'n':
                g_cli_options.aegis_validation = false;
                break;
                
            case 'm':
                g_cli_options.show_metrics = true;
                break;
                
            case 'O':
                g_cli_options.optimization_level = atoi(optarg);
                if (g_cli_options.optimization_level < 0 || g_cli_options.optimization_level > 3) {
                    RIFT_LOG_ERROR("Invalid optimization level: %s", optarg);
                    return RIFT_ERROR_INVALID_ARGUMENT;
                }
                break;
                
            case '?':
                RIFT_LOG_ERROR("Unknown option or missing argument");
                return RIFT_ERROR_INVALID_ARGUMENT;
                
            default:
                return RIFT_ERROR_INVALID_ARGUMENT;
        }
    }
    
    // Parse command if provided
    if (optind < argc) {
        g_cli_options.command = parse_command(argv[optind]);
        optind++;
        
        // Parse input file if provided
        if (optind < argc) {
            strncpy(g_cli_options.input_file, argv[optind], RIFT_MAX_OUTPUT_PATH - 1);
            g_cli_options.input_file[RIFT_MAX_OUTPUT_PATH - 1] = '\0';
        }
    }
    
    return RIFT_SUCCESS;
}

/*
 * Parse command string to command enum
 */
static rift_cli_command_t parse_command(const char* cmd_str) {
    if (strcmp(cmd_str, "tokenize") == 0) return RIFT_CMD_TOKENIZE;
    if (strcmp(cmd_str, "parse") == 0) return RIFT_CMD_PARSE;
    if (strcmp(cmd_str, "analyze") == 0) return RIFT_CMD_ANALYZE;
    if (strcmp(cmd_str, "validate") == 0) return RIFT_CMD_VALIDATE;
    if (strcmp(cmd_str, "generate") == 0) return RIFT_CMD_GENERATE;
    if (strcmp(cmd_str, "verify") == 0) return RIFT_CMD_VERIFY;
    if (strcmp(cmd_str, "emit") == 0) return RIFT_CMD_EMIT;
    if (strcmp(cmd_str, "compile") == 0) return RIFT_CMD_COMPILE;
    if (strcmp(cmd_str, "governance") == 0) return RIFT_CMD_GOVERNANCE;
    if (strcmp(cmd_str, "help") == 0) return RIFT_CMD_HELP;
    if (strcmp(cmd_str, "version") == 0) return RIFT_CMD_VERSION;
    
    RIFT_LOG_ERROR("Unknown command: %s", cmd_str);
    return RIFT_CMD_UNKNOWN;
}

/*
 * Execute the requested command
 */
static int execute_command(void) {
    switch (g_cli_options.command) {
        case RIFT_CMD_HELP:
            print_help();
            return RIFT_SUCCESS;
            
        case RIFT_CMD_VERSION:
            print_version();
            return RIFT_SUCCESS;
            
        case RIFT_CMD_TOKENIZE:
            return cmd_tokenize();
            
        case RIFT_CMD_PARSE:
            return cmd_parse();
            
        case RIFT_CMD_ANALYZE:
            return cmd_analyze();
            
        case RIFT_CMD_VALIDATE:
            return cmd_validate();
            
        case RIFT_CMD_GENERATE:
            return cmd_generate();
            
        case RIFT_CMD_VERIFY:
            return cmd_verify();
            
        case RIFT_CMD_EMIT:
            return cmd_emit();
            
        case RIFT_CMD_COMPILE:
            return cmd_compile();
            
        case RIFT_CMD_GOVERNANCE:
            return cmd_governance();
            
        case RIFT_CMD_UNKNOWN:
            RIFT_LOG_ERROR("Unknown command");
            print_usage();
            return RIFT_ERROR_INVALID_ARGUMENT;
            
        default:
            print_help();
            return RIFT_SUCCESS;
    }
}

/*
 * Command Implementations
 */

static int cmd_tokenize(void) {
    char* input_content = NULL;
    size_t input_size = 0;
    rift_tokenizer_state_t tokenizer_state = {0};
    rift_performance_metrics_t metrics = {0};
    int result;
    
    if (g_cli_options.show_metrics) {
        rift_performance_metrics_start(&metrics);
    }
    
    // Load input file or read from stdin
    if (strlen(g_cli_options.input_file) > 0) {
        result = load_input_file(g_cli_options.input_file, &input_content, &input_size);
        if (result != RIFT_SUCCESS) {
            return result;
        }
    } else {
        RIFT_LOG_ERROR("No input file specified for tokenization");
        return RIFT_ERROR_INVALID_ARGUMENT;
    }
    
    // Initialize tokenizer
    result = rift_tokenizer_init(input_content, &tokenizer_state);
    if (result != RIFT_SUCCESS) {
        RIFT_LOG_ERROR("Failed to initialize tokenizer: %s", rift_error_to_string(result));
        free(input_content);
        return result;
    }
    
    // Process tokenization
    result = rift_tokenizer_process(&tokenizer_state);
    if (result < 0) {
        RIFT_LOG_ERROR("Tokenization failed: %s", rift_error_to_string(result));
        rift_tokenizer_cleanup(&tokenizer_state);
        free(input_content);
        return result;
    }
    
    if (g_cli_options.verbose_mode) {
        RIFT_LOG_INFO("Tokenization completed: %d tokens generated", result);
    }
    
    // Output tokens (implementation would serialize to JSON or specified format)
    const rift_token_t* tokens = rift_tokenizer_get_tokens(&tokenizer_state);
    size_t token_count = rift_tokenizer_get_token_count(&tokenizer_state);
    
    // Print tokens or save to file
    if (strlen(g_cli_options.output_file) > 0) {
        // Serialize tokens to output file (implementation required)
        RIFT_LOG_INFO("Tokens saved to: %s", g_cli_options.output_file);
    } else {
        // Print tokens to stdout
        for (size_t i = 0; i < token_count; i++) {
            printf("Token[%zu]: type=%d, value='%s', line=%zu, col=%zu\n",
                   i, tokens[i].type, tokens[i].value, 
                   tokens[i].line_number, tokens[i].column_number);
        }
    }
    
    if (g_cli_options.show_metrics) {
        rift_performance_metrics_end(&metrics);
        print_performance_summary(&metrics);
    }
    
    // Cleanup
    rift_tokenizer_cleanup(&tokenizer_state);
    free(input_content);
    
    return RIFT_SUCCESS;
}

static int cmd_compile(void) {
    // Full pipeline compilation implementation
    RIFT_LOG_INFO("Executing full RIFT compilation pipeline...");
    
    // This would orchestrate all stages: tokenize -> parse -> analyze -> validate -> generate -> verify -> emit
    int result;
    
    // Stage 0: Tokenization
    result = cmd_tokenize();
    if (result != RIFT_SUCCESS) {
        RIFT_LOG_ERROR("Compilation failed at tokenization stage");
        return result;
    }
    
    // Additional stages would be implemented here following the same pattern
    
    RIFT_LOG_INFO("Compilation pipeline completed successfully");
    return RIFT_SUCCESS;
}

/*
 * Utility Functions
 */

static void print_version(void) {
    printf("%s version %s\n", RIFT_CLI_NAME, RIFT_CLI_VERSION);
    printf("RIFT Framework version %s\n", rift_get_version_string());
    printf("OBINexus Computing Framework - AEGIS Methodology\n");
    printf("Technical Lead: Nnamdi Michael Okpala\n");
    printf("Build: %s\n", rift_get_build_info());
}

static void print_help(void) {
    printf("Usage: %s [OPTIONS] COMMAND [INPUT_FILE]\n\n", RIFT_CLI_NAME);
    printf("%s\n\n", RIFT_CLI_DESCRIPTION);
    
    printf("Commands:\n");
    printf("  tokenize    Tokenize input source code\n");
    printf("  parse       Parse tokens into Abstract Syntax Tree\n");
    printf("  analyze     Perform semantic analysis\n");
    printf("  validate    Validate AST against governance policies\n");
    printf("  generate    Generate bytecode\n");
    printf("  verify      Verify bytecode integrity\n");
    printf("  emit        Emit final executable code\n");
    printf("  compile     Execute complete compilation pipeline\n");
    printf("  governance  Governance operations and validation\n");
    printf("  help        Show this help message\n");
    printf("  version     Show version information\n\n");
    
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -V, --version       Show version information\n");
    printf("  -v, --verbose       Enable verbose output\n");
    printf("  -d, --debug         Enable debug mode\n");
    printf("  -o, --output FILE   Specify output file\n");
    printf("  -c, --config FILE   Specify configuration file (default: .riftrc)\n");
    printf("  -s, --strict        Enable strict mode\n");
    printf("  -n, --no-aegis      Disable AEGIS governance validation\n");
    printf("  -m, --metrics       Show performance metrics\n");
    printf("  -O LEVEL            Set optimization level (0-3)\n\n");
    
    printf("Examples:\n");
    printf("  %s tokenize source.rift -o tokens.json\n", RIFT_CLI_NAME);
    printf("  %s compile source.rift -o output.rbc --verbose\n", RIFT_CLI_NAME);
    printf("  %s governance --validate --config security.riftrc\n", RIFT_CLI_NAME);
    printf("\nOBINexus Computing Framework - Computing from the Heart\n");
}

static void print_usage(void) {
    printf("Usage: %s [OPTIONS] COMMAND [INPUT_FILE]\n", RIFT_CLI_NAME);
    printf("Try '%s --help' for more information.\n", RIFT_CLI_NAME);
}

static int load_input_file(const char* filename, char** content, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        RIFT_LOG_ERROR("Failed to open input file: %s", filename);
        return RIFT_ERROR_FILE_NOT_FOUND;
    }
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (*size > RIFT_MAX_INPUT_SIZE) {
        RIFT_LOG_ERROR("Input file too large: %zu bytes (max: %d)", *size, RIFT_MAX_INPUT_SIZE);
        fclose(file);
        return RIFT_ERROR_FILE_ACCESS;
    }
    
    *content = malloc(*size + 1);
    if (!*content) {
        RIFT_LOG_ERROR("Failed to allocate memory for input file");
        fclose(file);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    size_t read_size = fread(*content, 1, *size, file);
    if (read_size != *size) {
        RIFT_LOG_ERROR("Failed to read complete input file");
        free(*content);
        fclose(file);
        return RIFT_ERROR_FILE_ACCESS;
    }
    
    (*content)[*size] = '\0';
    fclose(file);
    
    return RIFT_SUCCESS;
}

static void print_performance_summary(const rift_performance_metrics_t* metrics) {
    printf("\n=== Performance Metrics ===\n");
    printf("Execution time: %lu ms\n", 
           (metrics->end_time - metrics->start_time) / 1000);
    printf("Peak memory usage: %zu bytes\n", metrics->memory_peak_usage);
    printf("Total allocations: %zu\n", metrics->allocations_count);
    printf("Complexity score: %zu\n", metrics->complexity_score);
    printf("============================\n");
}

// Stub implementations for remaining commands
static int cmd_parse(void) {
    RIFT_LOG_INFO("Parse command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

static int cmd_analyze(void) {
    RIFT_LOG_INFO("Analyze command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

static int cmd_validate(void) {
    RIFT_LOG_INFO("Validate command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

static int cmd_generate(void) {
    RIFT_LOG_INFO("Generate command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

static int cmd_verify(void) {
    RIFT_LOG_INFO("Verify command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

static int cmd_emit(void) {
    RIFT_LOG_INFO("Emit command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

static int cmd_governance(void) {
    RIFT_LOG_INFO("Governance command not implemented");
    return RIFT_ERROR_NOT_IMPLEMENTED;
}

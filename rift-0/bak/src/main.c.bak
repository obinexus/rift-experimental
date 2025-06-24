/*
 * =================================================================
 * main.c - RIFT-0 CLI Application
 * RIFT: RIFT Is a Flexible Translator
 * Component: Command-line interface for tokenizer stage
 * OBINexus Computing Framework - Stage 0 CLI
 * 
 * Author: OBINexus Nnamdi Michael Okpala
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * =================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>

#include "rift-0/core/tokenizer.h"
#include "rift-0/cli/cli_interface.h"

/* =================================================================
 * CLI CONFIGURATION & CONSTANTS
 * =================================================================
 */

#define CLI_VERSION "1.0.0"
#define CLI_DESCRIPTION "RIFT-0 Tokenizer Stage - DFA-based lexical analysis"
#define CLI_AUTHOR "OBINexus Nnamdi Michael Okpala"

#define MAX_INPUT_SIZE (1024 * 1024)  /* 1MB input limit */
#define DEFAULT_OUTPUT_FORMAT "tokens"

/* =================================================================
 * CLI OPTION STRUCTURES
 * =================================================================
 */

typedef enum {
    OUTPUT_FORMAT_TOKENS,
    OUTPUT_FORMAT_JSON,
    OUTPUT_FORMAT_XML,
    OUTPUT_FORMAT_CSV,
    OUTPUT_FORMAT_BINARY
} OutputFormat;

typedef struct {
    const char* input_file;
    const char* output_file;
    OutputFormat output_format;
    bool verbose;
    bool debug;
    bool validate_only;
    bool show_stats;
    bool enable_threading;
    const char* regex_pattern;
    TokenFlags regex_flags;
    bool interactive_mode;
    size_t buffer_size;
} CLIOptions;

/* =================================================================
 * FUNCTION PROTOTYPES
 * =================================================================
 */

static void print_version(void);
static void print_help(const char* program_name);
static void print_banner(void);
static int parse_arguments(int argc, char* argv[], CLIOptions* options);
static int process_file(const CLIOptions* options);
static int interactive_mode(const CLIOptions* options);
static int validate_dfa_pattern(const char* pattern, TokenFlags flags);
static void output_tokens(const TokenizerContext* ctx, const CLIOptions* options);
static void output_statistics(const TokenizerContext* ctx);
static TokenFlags parse_regex_flags(const char* flag_string);
static const char* get_file_extension(OutputFormat format);

/* =================================================================
 * MAIN ENTRY POINT
 * =================================================================
 */

int main(int argc, char* argv[]) {
    CLIOptions options = {
        .input_file = NULL,
        .output_file = NULL,
        .output_format = OUTPUT_FORMAT_TOKENS,
        .verbose = false,
        .debug = false,
        .validate_only = false,
        .show_stats = false,
        .enable_threading = false,
        .regex_pattern = NULL,
        .regex_flags = TOKEN_FLAG_NONE,
        .interactive_mode = false,
        .buffer_size = RIFT_DEFAULT_TOKEN_CAPACITY
    };
    
    /* Parse command line arguments */
    int parse_result = parse_arguments(argc, argv, &options);
    if (parse_result != 0) {
        return parse_result;
    }
    
    /* Interactive mode takes precedence */
    if (options.interactive_mode) {
        return interactive_mode(&options);
    }
    
    /* Validate DFA pattern if provided */
    if (options.regex_pattern) {
        if (options.validate_only) {
            return validate_dfa_pattern(options.regex_pattern, options.regex_flags);
        }
    }
    
    /* Process input file */
    if (options.input_file) {
        return process_file(&options);
    }
    
    /* No input specified */
    fprintf(stderr, "Error: No input file specified. Use --help for usage information.\n");
    return EXIT_FAILURE;
}

/* =================================================================
 * ARGUMENT PARSING
 * =================================================================
 */

static int parse_arguments(int argc, char* argv[], CLIOptions* options) {
    static struct option long_options[] = {
        {"help",        no_argument,       0, 'h'},
        {"version",     no_argument,       0, 'V'},
        {"verbose",     no_argument,       0, 'v'},
        {"debug",       no_argument,       0, 'd'},
        {"output",      required_argument, 0, 'o'},
        {"format",      required_argument, 0, 'f'},
        {"validate",    no_argument,       0, 'c'},
        {"stats",       no_argument,       0, 's'},
        {"threading",   no_argument,       0, 't'},
        {"pattern",     required_argument, 0, 'p'},
        {"flags",       required_argument, 0, 'F'},
        {"interactive", no_argument,       0, 'i'},
        {"buffer-size", required_argument, 0, 'b'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hVvdo:f:cstp:F:ib:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                print_help(argv[0]);
                return EXIT_SUCCESS;
                
            case 'V':
                print_version();
                return EXIT_SUCCESS;
                
            case 'v':
                options->verbose = true;
                break;
                
            case 'd':
                options->debug = true;
                options->verbose = true; /* Debug implies verbose */
                break;
                
            case 'o':
                options->output_file = optarg;
                break;
                
            case 'f':
                if (strcmp(optarg, "tokens") == 0) {
                    options->output_format = OUTPUT_FORMAT_TOKENS;
                } else if (strcmp(optarg, "json") == 0) {
                    options->output_format = OUTPUT_FORMAT_JSON;
                } else if (strcmp(optarg, "xml") == 0) {
                    options->output_format = OUTPUT_FORMAT_XML;
                } else if (strcmp(optarg, "csv") == 0) {
                    options->output_format = OUTPUT_FORMAT_CSV;
                } else if (strcmp(optarg, "binary") == 0) {
                    options->output_format = OUTPUT_FORMAT_BINARY;
                } else {
                    fprintf(stderr, "Error: Unknown output format '%s'\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
                
            case 'c':
                options->validate_only = true;
                break;
                
            case 's':
                options->show_stats = true;
                break;
                
            case 't':
                options->enable_threading = true;
                break;
                
            case 'p':
                options->regex_pattern = optarg;
                break;
                
            case 'F':
                options->regex_flags = parse_regex_flags(optarg);
                break;
                
            case 'i':
                options->interactive_mode = true;
                break;
                
            case 'b':
                options->buffer_size = (size_t)atol(optarg);
                if (options->buffer_size == 0) {
                    fprintf(stderr, "Error: Invalid buffer size '%s'\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
                
            case '?':
                return EXIT_FAILURE;
                
            default:
                fprintf(stderr, "Error: Unknown option\n");
                return EXIT_FAILURE;
        }
    }
    
    /* Get input file from remaining arguments */
    if (optind < argc && !options->interactive_mode) {
        options->input_file = argv[optind];
    }
    
    return 0;
}

/* =================================================================
 * FILE PROCESSING
 * =================================================================
 */

static int process_file(const CLIOptions* options) {
    if (options->verbose) {
        print_banner();
        printf("Processing file: %s\n", options->input_file);
    }
    
    /* Create tokenizer context */
    TokenizerContext* ctx = rift_tokenizer_create(options->buffer_size);
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create tokenizer context\n");
        return EXIT_FAILURE;
    }
    
    /* Enable thread safety if requested */
    if (options->enable_threading) {
        if (!rift_tokenizer_enable_thread_safety(ctx)) {
            fprintf(stderr, "Warning: Failed to enable thread safety\n");
        } else if (options->verbose) {
            printf("Thread safety enabled for Gosilang integration\n");
        }
    }
    
    /* Load input file */
    if (!rift_tokenizer_set_input_file(ctx, options->input_file)) {
        fprintf(stderr, "Error: Failed to load input file: %s\n", 
                rift_tokenizer_get_error(ctx));
        rift_tokenizer_destroy(ctx);
        return EXIT_FAILURE;
    }
    
    /* Add custom regex pattern if provided */
    if (options->regex_pattern) {
        if (options->verbose) {
            printf("Applying custom regex pattern: %s\n", options->regex_pattern);
        }
        
        if (!rift_tokenizer_cache_pattern(ctx, "custom", 
                                          options->regex_pattern, 
                                          options->regex_flags)) {
            fprintf(stderr, "Error: Failed to compile regex pattern: %s\n",
                    rift_tokenizer_get_error(ctx));
            rift_tokenizer_destroy(ctx);
            return EXIT_FAILURE;
        }
    }
    
    /* Process tokens */
    if (options->verbose) {
        printf("Starting tokenization process...\n");
    }
    
    if (!rift_tokenizer_process(ctx)) {
        fprintf(stderr, "Error: Tokenization failed: %s\n",
                rift_tokenizer_get_error(ctx));
        rift_tokenizer_destroy(ctx);
        return EXIT_FAILURE;
    }
    
    if (options->verbose) {
        size_t token_count;
        rift_tokenizer_get_tokens(ctx, &token_count);
        printf("Tokenization completed. Generated %zu tokens.\n", token_count);
    }
    
    /* Output results */
    output_tokens(ctx, options);
    
    /* Show statistics if requested */
    if (options->show_stats) {
        output_statistics(ctx);
    }
    
    /* Cleanup */
    rift_tokenizer_destroy(ctx);
    
    if (options->verbose) {
        printf("Processing completed successfully.\n");
    }
    
    return EXIT_SUCCESS;
}

/* =================================================================
 * INTERACTIVE MODE
 * =================================================================
 */

static int interactive_mode(const CLIOptions* options) {
    print_banner();
    printf("RIFT-0 Interactive Tokenizer Mode\n");
    printf("Type 'help' for commands, 'quit' to exit.\n\n");
    
    TokenizerContext* ctx = rift_tokenizer_create(options->buffer_size);
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create tokenizer context\n");
        return EXIT_FAILURE;
    }
    
    if (options->enable_threading) {
        rift_tokenizer_enable_thread_safety(ctx);
    }
    
    char input_buffer[1024];
    bool running = true;
    
    while (running) {
        printf("rift-0> ");
        fflush(stdout);
        
        if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
            break;
        }
        
        /* Remove newline */
        size_t len = strlen(input_buffer);
        if (len > 0 && input_buffer[len-1] == '\n') {
            input_buffer[len-1] = '\0';
            len--;
        }
        
        if (len == 0) continue;
        
        /* Process commands */
        if (strcmp(input_buffer, "quit") == 0 || strcmp(input_buffer, "exit") == 0) {
            running = false;
        } else if (strcmp(input_buffer, "help") == 0) {
            printf("Interactive Commands:\n");
            printf("  help          - Show this help\n");
            printf("  quit/exit     - Exit interactive mode\n");
            printf("  stats         - Show tokenizer statistics\n");
            printf("  reset         - Reset tokenizer state\n");
            printf("  validate      - Validate current DFA state\n");
            printf("  <text>        - Tokenize the input text\n");
        } else if (strcmp(input_buffer, "stats") == 0) {
            output_statistics(ctx);
        } else if (strcmp(input_buffer, "reset") == 0) {
            rift_tokenizer_reset(ctx);
            printf("Tokenizer state reset.\n");
        } else if (strcmp(input_buffer, "validate") == 0) {
            bool valid = rift_tokenizer_validate_dfa(ctx);
            printf("DFA validation: %s\n", valid ? "PASS" : "FAIL");
        } else {
            /* Tokenize the input */
            rift_tokenizer_reset(ctx);
            
            if (rift_tokenizer_set_input(ctx, input_buffer, len)) {
                if (rift_tokenizer_process(ctx)) {
                    size_t token_count;
                    TokenTriplet* tokens = rift_tokenizer_get_tokens(ctx, &token_count);
                    
                    printf("Tokens (%zu):\n", token_count);
                    for (size_t i = 0; i < token_count; i++) {
                        printf("  [%zu] Type: %s, Ptr: %u, Value: 0x%02X\n",
                               i, rift_token_type_name(tokens[i].type),
                               tokens[i].mem_ptr, tokens[i].value);
                    }
                } else {
                    printf("Error: %s\n", rift_tokenizer_get_error(ctx));
                }
            } else {
                printf("Error: %s\n", rift_tokenizer_get_error(ctx));
            }
        }
    }
    
    printf("Goodbye!\n");
    rift_tokenizer_destroy(ctx);
    return EXIT_SUCCESS;
}

/* =================================================================
 * OUTPUT FUNCTIONS
 * =================================================================
 */

static void output_tokens(const TokenizerContext* ctx, const CLIOptions* options) {
    size_t token_count;
    TokenTriplet* tokens = rift_tokenizer_get_tokens(ctx, &token_count);
    
    FILE* output = stdout;
    if (options->output_file) {
        output = fopen(options->output_file, "w");
        if (!output) {
            fprintf(stderr, "Error: Cannot open output file '%s': %s\n",
                    options->output_file, strerror(errno));
            return;
        }
    }
    
    switch (options->output_format) {
        case OUTPUT_FORMAT_TOKENS:
            fprintf(output, "# RIFT-0 Token Output\n");
            fprintf(output, "# Format: Index Type MemPtr Value Flags\n");
            for (size_t i = 0; i < token_count; i++) {
                fprintf(output, "%zu %s %u 0x%02X %s\n",
                        i, rift_token_type_name(tokens[i].type),
                        tokens[i].mem_ptr, tokens[i].value,
                        rift_token_flags_string(tokens[i].value));
            }
            break;
            
        case OUTPUT_FORMAT_JSON:
            fprintf(output, "{\n");
            fprintf(output, "  \"version\": \"%s\",\n", CLI_VERSION);
            fprintf(output, "  \"tokenizer\": \"rift-0\",\n");
            fprintf(output, "  \"token_count\": %zu,\n", token_count);
            fprintf(output, "  \"tokens\": [\n");
            for (size_t i = 0; i < token_count; i++) {
                fprintf(output, "    {\n");
                fprintf(output, "      \"index\": %zu,\n", i);
                fprintf(output, "      \"type\": \"%s\",\n", rift_token_type_name(tokens[i].type));
                fprintf(output, "      \"mem_ptr\": %u,\n", tokens[i].mem_ptr);
                fprintf(output, "      \"value\": %u,\n", tokens[i].value);
                fprintf(output, "      \"flags\": \"%s\"\n", rift_token_flags_string(tokens[i].value));
                fprintf(output, "    }%s\n", (i < token_count - 1) ? "," : "");
            }
            fprintf(output, "  ]\n");
            fprintf(output, "}\n");
            break;
            
        case OUTPUT_FORMAT_CSV:
            fprintf(output, "Index,Type,MemPtr,Value,Flags\n");
            for (size_t i = 0; i < token_count; i++) {
                fprintf(output, "%zu,%s,%u,%u,\"%s\"\n",
                        i, rift_token_type_name(tokens[i].type),
                        tokens[i].mem_ptr, tokens[i].value,
                        rift_token_flags_string(tokens[i].value));
            }
            break;
            
        case OUTPUT_FORMAT_BINARY:
            fwrite(tokens, sizeof(TokenTriplet), token_count, output);
            break;
            
        case OUTPUT_FORMAT_XML:
            fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            fprintf(output, "<rift_tokens version=\"%s\" count=\"%zu\">\n", CLI_VERSION, token_count);
            for (size_t i = 0; i < token_count; i++) {
                fprintf(output, "  <token index=\"%zu\" type=\"%s\" mem_ptr=\"%u\" value=\"%u\" flags=\"%s\"/>\n",
                        i, rift_token_type_name(tokens[i].type),
                        tokens[i].mem_ptr, tokens[i].value,
                        rift_token_flags_string(tokens[i].value));
            }
            fprintf(output, "</rift_tokens>\n");
            break;
    }
    
    if (options->output_file) {
        fclose(output);
    }
}

static void output_statistics(const TokenizerContext* ctx) {
    TokenizerStats stats = rift_tokenizer_get_stats(ctx);
    
    printf("\n=== RIFT-0 Tokenizer Statistics ===\n");
    printf("Total tokens generated:     %lu\n", stats.total_tokens);
    printf("Total characters processed: %lu\n", stats.total_characters);
    printf("Processing time (ns):       %lu\n", stats.processing_time_ns);
    printf("DFA state transitions:      %lu\n", stats.dfa_transitions);
    printf("Pattern cache hits:         %lu\n", stats.cache_hits);
    printf("Pattern cache misses:       %lu\n", stats.cache_misses);
    
    if (stats.total_characters > 0) {
        printf("Throughput (chars/sec):     %.2f\n", 
               (double)stats.total_characters * 1000000000.0 / stats.processing_time_ns);
    }
    
    if (stats.cache_hits + stats.cache_misses > 0) {
        double hit_ratio = (double)stats.cache_hits / (stats.cache_hits + stats.cache_misses) * 100.0;
        printf("Cache hit ratio:            %.1f%%\n", hit_ratio);
    }
    
    printf("=====================================\n\n");
}

/* =================================================================
 * UTILITY FUNCTIONS
 * =================================================================
 */

static TokenFlags parse_regex_flags(const char* flag_string) {
    TokenFlags flags = TOKEN_FLAG_NONE;
    
    for (const char* p = flag_string; *p; p++) {
        switch (*p) {
            case 'g': flags |= TOKEN_FLAG_GLOBAL; break;
            case 'm': flags |= TOKEN_FLAG_MULTILINE; break;
            case 'i': flags |= TOKEN_FLAG_IGNORECASE; break;
            case 't': flags |= TOKEN_FLAG_TOPDOWN; break;
            case 'b': flags |= TOKEN_FLAG_BOTTOMUP; break;
            default:
                fprintf(stderr, "Warning: Unknown regex flag '%c'\n", *p);
                break;
        }
    }
    
    return flags;
}

static int validate_dfa_pattern(const char* pattern, TokenFlags flags) {
    printf("Validating DFA pattern: %s\n", pattern);
    printf("Flags: %s\n", rift_token_flags_string(flags));
    
    RegexComposition* regex = rift_regex_compile(pattern, flags);
    if (!regex) {
        printf("VALIDATION FAILED: Pattern compilation error\n");
        return EXIT_FAILURE;
    }
    
    printf("VALIDATION PASSED: Pattern compiled successfully\n");
    rift_regex_destroy(regex);
    return EXIT_SUCCESS;
}

static void print_version(void) {
    printf("rift-0 %s\n", CLI_VERSION);
    printf("%s\n", CLI_DESCRIPTION);
    printf("Author: %s\n", CLI_AUTHOR);
    printf("Build: %s\n", rift_tokenizer_build_info());
    printf("\nRIFT Framework Features:\n");
    printf("  DFA Support:      %s\n", rift_tokenizer_has_dfa_support() ? "YES" : "NO");
    printf("  Regex Compose:    %s\n", rift_tokenizer_has_regex_compose() ? "YES" : "NO");
    printf("  Thread Safety:    %s\n", rift_tokenizer_has_thread_safety() ? "YES" : "NO");
    printf("  Pattern Caching:  %s\n", rift_tokenizer_has_caching() ? "YES" : "NO");
    printf("\nOBINexus Computing - Computing from the Heart\n");
}

static void print_banner(void) {
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║                   RIFT-0 Tokenizer                ║\n");
    printf("║              RIFT Is a Flexible Translator        ║\n");
    printf("║         OBINexus Computing Framework v%s        ║\n", CLI_VERSION);
    printf("╚════════════════════════════════════════════════════╝\n");
    printf("Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
    printf("Build: nlink → polybuild (AEGIS Framework)\n\n");
}

static void print_help(const char* program_name) {
    print_banner();
    printf("USAGE:\n");
    printf("  %s [OPTIONS] <input-file>\n", program_name);
    printf("  %s [OPTIONS] --interactive\n", program_name);
    printf("\n");
    
    printf("DESCRIPTION:\n");
    printf("  RIFT-0 implements DFA-based tokenization with regex composition\n");
    printf("  support. Supports R\"\" and R'' syntax with boolean operations.\n");
    printf("\n");
    
    printf("OPTIONS:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -V, --version           Show version information\n");
    printf("  -v, --verbose           Enable verbose output\n");
    printf("  -d, --debug             Enable debug mode (implies verbose)\n");
    printf("  -o, --output FILE       Output file (default: stdout)\n");
    printf("  -f, --format FORMAT     Output format (tokens|json|xml|csv|binary)\n");
    printf("  -c, --validate          Validate pattern only (with --pattern)\n");
    printf("  -s, --stats             Show performance statistics\n");
    printf("  -t, --threading         Enable thread safety (for Gosilang)\n");
    printf("  -p, --pattern REGEX     Custom regex pattern to apply\n");
    printf("  -F, --flags FLAGS       Regex flags (gmitb)\n");
    printf("  -i, --interactive       Enter interactive mode\n");
    printf("  -b, --buffer-size SIZE  Token buffer size (default: %d)\n", RIFT_DEFAULT_TOKEN_CAPACITY);
    printf("\n");
    
    printf("REGEX FLAGS:\n");
    printf("  g  Global matching\n");
    printf("  m  Multiline mode\n");
    printf("  i  Case insensitive\n");
    printf("  t  Top-down evaluation\n");
    printf("  b  Bottom-up evaluation\n");
    printf("\n");
    
    printf("EXAMPLES:\n");
    printf("  %s input.rift                           # Basic tokenization\n", program_name);
    printf("  %s -v -s input.rift                     # Verbose with stats\n", program_name);
    printf("  %s -f json -o tokens.json input.rift    # JSON output\n", program_name);
    printf("  %s -p 'R\"/[A-Z]+/gi\"' input.rift        # Custom pattern\n", program_name);
    printf("  %s -i                                   # Interactive mode\n", program_name);
    printf("\n");
    
    printf("AUTHOR:\n");
    printf("  %s\n", CLI_AUTHOR);
    printf("  OBINexus Computing - Computing from the Heart\n");
}
#include "../include/rift0/rift.h"

RiftConfig* rift_config_create(void) {
    RiftConfig* config = calloc(1, sizeof(RiftConfig));
    if (!config) return NULL;
    
    // Set defaults
    config->output_dir = strdup("./output");
    config->stage_name = strdup("stage0");
    config->debug_mode = false;
    config->verbose = false;
    config->log_level = strdup("INFO");
    
    if (!config->output_dir || !config->stage_name || !config->log_level) {
        rift_config_destroy(config);
        return NULL;
    }
    
    return config;
}

void rift_config_destroy(RiftConfig* config) {
    if (!config) return;
    
    free(config->output_dir);
    free(config->stage_name);
    free(config->log_level);
    free(config);
}

RiftResult rift_config_load(RiftConfig* config, const char* config_file) {
    if (!config || !config_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(config_file, "r");
    if (!file) return RIFT_ERROR_IO;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Simple key=value parser
        char* equals = strchr(line, '=');
        if (!equals) continue;
        
        *equals = '\0';
        char* key = line;
        char* value = equals + 1;
        
        // Trim whitespace
        while (isspace(*key)) key++;
        while (isspace(*value)) value++;
        
        char* end = value + strlen(value) - 1;
        while (end > value && isspace(*end)) *end-- = '\0';
        
        // Parse known keys
        if (strcmp(key, "output_dir") == 0) {
            free(config->output_dir);
            config->output_dir = strdup(value);
        } else if (strcmp(key, "debug_mode") == 0) {
            config->debug_mode = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "verbose") == 0) {
            config->verbose = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "log_level") == 0) {
            free(config->log_level);
            config->log_level = strdup(value);
        }
    }
    
    fclose(file);
    return RIFT_SUCCESS;
}

RiftResult rift_config_save(RiftConfig* config, const char* config_file) {
    if (!config || !config_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(config_file, "w");
    if (!file) return RIFT_ERROR_IO;
    
    fprintf(file, "# RIFT Configuration File\n");
    fprintf(file, "output_dir=%s\n", config->output_dir);
    fprintf(file, "debug_mode=%s\n", config->debug_mode ? "true" : "false");
    fprintf(file, "verbose=%s\n", config->verbose ? "true" : "false");
    fprintf(file, "log_level=%s\n", config->log_level);
    
    fclose(file);
    return RIFT_SUCCESS;
}

// IR Management
RiftIR* rift_ir_create(const char* source_file) {
    RiftIR* ir = calloc(1, sizeof(RiftIR));
    if (!ir) return NULL;
    
    ir->stage = strdup("rift.0");
    ir->version = strdup("1.0.0");
    ir->source_file = strdup(source_file ? source_file : "unknown");
    ir->metadata = strdup("# RIFT Stage 0 IR Output\n");
    
    if (!ir->stage || !ir->version || !ir->source_file || !ir->metadata) {
        rift_ir_destroy(ir);
        return NULL;
    }
    
    return ir;
}

void rift_ir_destroy(RiftIR* ir) {
    if (!ir) return;
    
    free(ir->stage);
    free(ir->version);
    free(ir->source_file);
    free(ir->metadata);
    
    for (size_t i = 0; i < ir->token_count; i++) {
        rift_token_destroy(ir->tokens[i]);
    }
    free(ir->tokens);
    
    free(ir);
}

RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file) {
    if (!ir || !output_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(output_file, "w");
    if (!file) return RIFT_ERROR_IO;
    
    // Write IR header
    fprintf(file, "# RIFT Intermediate Representation\n");
    fprintf(file, "stage=%s\n", ir->stage);
    fprintf(file, "version=%s\n", ir->version);
    fprintf(file, "source=%s\n", ir->source_file);
    fprintf(file, "token_count=%zu\n", ir->token_count);
    fprintf(file, "\n# Tokens\n");
    
    // Write tokens
    for (size_t i = 0; i < ir->token_count; i++) {
        RiftToken* token = ir->tokens[i];
        fprintf(file, "TOKEN %zu: type=\"%s\" value=\"%s\" pos=(%zu,%zu)\n",
                i, token->type, token->value, token->line, token->column);
    }
    
    fclose(file);
    return RIFT_SUCCESS;
}

// Utility functions
const char* rift_result_string(RiftResult result) {
    switch (result) {
        case RIFT_SUCCESS: return "Success";
        case RIFT_ERROR_NULL_POINTER: return "Null pointer error";
        case RIFT_ERROR_MEMORY_ALLOCATION: return "Memory allocation error";
        case RIFT_ERROR_PATTERN_INVALID: return "Invalid pattern";
        case RIFT_ERROR_COMPILATION_FAILED: return "Compilation failed";
        case RIFT_ERROR_IO: return "I/O error";
        default: return "Unknown error";
    }
}

void rift_print_version(void) {
    printf("RIFT Stage 0 (Tokenization/Lexical Analysis) v1.0.0\n");
    printf("AEGIS Framework - OBINexus Computing\n");
    printf("\"Structure is the final syntax.\"\n");
}

void rift_print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS] <input_file>\n", program_name);
    printf("\nOptions:\n");
    printf("  -o, --output DIR     Output directory (default: ./output)\n");
    printf("  -c, --config FILE    Configuration file (default: .riftrc)\n");
    printf("  -v, --verbose        Verbose output\n");
    printf("  -d, --debug          Debug mode\n");
    printf("  -h, --help           Show this help\n");
    printf("  --version            Show version information\n");
    printf("\nExamples:\n");
    printf("  %s example.rift\n", program_name);
    printf("  %s -o ./build -v source.rift\n", program_name);
    printf("  %s --config custom.riftrc input.rift\n", program_name);
}

RiftResult rift_process_file(const char* input_file, const char* output_file, 
                           RiftConfig* config) {
    if (!input_file || !output_file || !config) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    // Read input file
    FILE* file = fopen(input_file, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return RIFT_ERROR_IO;
    }
    
    // Read entire file into memory
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* input_content = malloc((size_t)file_size + 1);
    if (!input_content) {
        fclose(file);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    size_t bytes_read = fread(input_content, 1, (size_t)file_size, file);
    input_content[bytes_read] = '\0';
    fclose(file);
    
    // Create RIFT engine and process
    RiftEngine* engine = rift_engine_create();
    if (!engine) {
        free(input_content);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    // Add default patterns
    rift_automaton_add_state(engine->automaton, "R\"[A-Z0-9]+/gmi[t]\"", false);
    rift_automaton_add_state(engine->automaton, "R\"\\s+/gm[b]\"", false);
    rift_automaton_add_state(engine->automaton, "R\"[^A-Z0-9\\s]+/gmi[t]\"", true);
    
    RiftResult result = rift_engine_process_input(engine, input_content);
    if (result != RIFT_SUCCESS) {
        rift_engine_destroy(engine);
        free(input_content);
        return result;
    }
    
    // Create IR and collect tokens
    RiftIR* ir = rift_ir_create(input_file);
    if (!ir) {
        rift_engine_destroy(engine);
        free(input_content);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    // Tokenize and collect
    RiftToken* token;
    while ((token = rift_engine_next_token(engine)) != NULL) {
        rift_ir_add_token(ir, token);
        if (ir->token_count > 1000) { // Safety limit
            break;
        }
    }
    
    // Write IR output
    result = rift_ir_write_file(ir, output_file);
    
    if (config->verbose) {
        printf("Processed '%s' -> '%s' (%zu tokens)\n", 
               input_file, output_file, ir->token_count);
    }
    
    // Cleanup
    rift_ir_destroy(ir);
    rift_engine_destroy(engine);
    free(input_content);
    
    return result;
}

RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token) {
    if (!ir || !token) return RIFT_ERROR_NULL_POINTER;
    
    // Resize token array if needed
    if (ir->token_count >= 1000) { // Current capacity limit
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    if (!ir->tokens) {
        ir->tokens = calloc(1000, sizeof(RiftToken*));
        if (!ir->tokens) return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    ir->tokens[ir->token_count++] = token;
    return RIFT_SUCCESS;
}

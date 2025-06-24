// rift/src/commands/tokenize.c
#include <rift/cli/commands.h>
#include <rift/core/tokenizer/tokenizer.h>

rift_result_t rift_command_tokenize(const char *input_source, 
                                    const char *output_path,
                                    bool verbose) {
    rift_result_t result = RIFT_SUCCESS;
    rift_tokenizer_t tokenizer;
    rift_token_stream_t tokens;
    
    if (verbose) {
        printf("📝 Tokenization Stage - AEGIS RIFT Framework\n");
        printf("Input: %s\n", input_source ? input_source : "<stdin>");
        printf("Output: %s\n", output_path ? output_path : "<stdout>");
    }
    
    // Initialize tokenizer with AEGIS compliance
    result = rift_tokenizer_init(&tokenizer);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Tokenizer initialization failed\n");
        return result;
    }
    
    // Execute tokenization with memory alignment validation
    result = rift_tokenizer_process(&tokenizer, input_source, &tokens);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Tokenization failed\n");
        goto cleanup;
    }
    
    // Validate token schema compliance
    result = rift_tokenizer_validate_schema(&tokens);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Token schema validation failed\n");
        goto cleanup;
    }
    
    // Write output with systematic error handling
    result = rift_tokenizer_write_output(&tokens, output_path);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Output generation failed\n");
        goto cleanup;
    }
    
    if (verbose) {
        printf("✅ Tokenization completed successfully\n");
        printf("   Tokens generated: %zu\n", tokens.count);
        printf("   Memory alignment: %d-bit\n", tokens.memory_alignment);
    }
    
cleanup:
    rift_tokenizer_cleanup(&tokenizer);
    rift_token_stream_cleanup(&tokens);
    return result;
}
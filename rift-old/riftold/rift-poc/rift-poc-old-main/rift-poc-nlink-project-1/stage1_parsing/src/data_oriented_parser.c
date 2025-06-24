/**
 * @file data_oriented_parser.c
 * @brief AEGIS data-oriented parser implementation
 */

#include "aegis/parser.h"
#include "aegis/parse_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** tokens;
    size_t token_count;
    size_t current_token;
} TokenStream;

typedef struct {
    TokenStream* stream;
    ParseTreeBuilder* builder;
    bool zero_trust_mode;
} ParserContext;

ParserContext* parser_create_context(bool zero_trust_mode) {
    ParserContext* ctx = malloc(sizeof(ParserContext));
    if (!ctx) return NULL;
    
    ctx->stream = NULL;
    ctx->builder = parse_tree_builder_create();
    ctx->zero_trust_mode = zero_trust_mode;
    
    return ctx;
}

bool parser_load_tokens(ParserContext* ctx, const char* input_file) {
    FILE* file = fopen(input_file, "r");
    if (!file) return false;
    
    // Parse tokenized input
    char line[1024];
    size_t capacity = 100;
    ctx->stream = malloc(sizeof(TokenStream));
    ctx->stream->tokens = malloc(sizeof(char*) * capacity);
    ctx->stream->token_count = 0;
    ctx->stream->current_token = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "TOKEN_", 6) == 0) {
            // Extract token content
            char* colon = strchr(line, ':');
            if (colon) {
                char* token_content = colon + 2; // Skip ": "
                char* end = strchr(token_content, ' ');
                if (end) *end = '\0';
                
                if (ctx->stream->token_count >= capacity) {
                    capacity *= 2;
                    ctx->stream->tokens = realloc(ctx->stream->tokens, sizeof(char*) * capacity);
                }
                
                ctx->stream->tokens[ctx->stream->token_count++] = strdup(token_content);
            }
        }
    }
    
    fclose(file);
    return true;
}

ParseNode* parser_parse_expression(ParserContext* ctx) {
    if (ctx->stream->current_token >= ctx->stream->token_count) return NULL;
    
    ParseNode* left = parse_tree_create_leaf(ctx->builder, 
                                           ctx->stream->tokens[ctx->stream->current_token++]);
    
    // Handle operators
    if (ctx->stream->current_token < ctx->stream->token_count) {
        char* current = ctx->stream->tokens[ctx->stream->current_token];
        if (strcmp(current, "+") == 0 || strcmp(current, "-") == 0 || 
            strcmp(current, "*") == 0 || strcmp(current, "/") == 0) {
            
            ctx->stream->current_token++; // consume operator
            ParseNode* op = parse_tree_create_node(ctx->builder, current);
            ParseNode* right = parser_parse_expression(ctx);
            
            parse_tree_add_child(op, left);
            if (right) parse_tree_add_child(op, right);
            
            return op;
        }
    }
    
    return left;
}

bool parser_process_file(ParserContext* ctx, const char* input_file, const char* output_file) {
    if (!parser_load_tokens(ctx, input_file)) return false;
    
    FILE* output = fopen(output_file, "w");
    if (!output) return false;
    
    fprintf(output, "# RIFT Stage 1 Output - Data-Oriented Parse Tree\n");
    fprintf(output, "# Input: %s\n", input_file);
    fprintf(output, "# Zero Trust: %s\n", ctx->zero_trust_mode ? "ENABLED" : "DISABLED");
    fprintf(output, "# Methodology: Data-Oriented\n\n");
    
    // Parse the token stream
    ParseNode* root = parse_tree_create_node(ctx->builder, "PROGRAM");
    
    while (ctx->stream->current_token < ctx->stream->token_count) {
        ParseNode* expr = parser_parse_expression(ctx);
        if (expr) {
            parse_tree_add_child(root, expr);
        }
    }
    
    // Output parse tree
    parse_tree_print(root, output, 0);
    
    fclose(output);
    return true;
}

void parser_free_context(ParserContext* ctx) {
    if (!ctx) return;
    
    if (ctx->stream) {
        for (size_t i = 0; i < ctx->stream->token_count; i++) {
            free(ctx->stream->tokens[i]);
        }
        free(ctx->stream->tokens);
        free(ctx->stream);
    }
    
    parse_tree_builder_free(ctx->builder);
    free(ctx);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.rift.0> <output.rift.1> [--zero-trust]\n", argv[0]);
        return 1;
    }
    
    bool zero_trust = (argc > 3 && strcmp(argv[3], "--zero-trust") == 0);
    
    ParserContext* ctx = parser_create_context(zero_trust);
    if (!ctx) {
        fprintf(stderr, "Failed to create parser context\n");
        return 1;
    }
    
    bool success = parser_process_file(ctx, argv[1], argv[2]);
    parser_free_context(ctx);
    
    if (success) {
        printf("Stage 1 parsing completed: %s\n", argv[2]);
        return 0;
    } else {
        fprintf(stderr, "Stage 1 parsing failed\n");
        return 1;
    }
}

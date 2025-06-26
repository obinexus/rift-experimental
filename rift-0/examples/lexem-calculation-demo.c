/**
 * =================================================================
 * lexem-calculation-demo.c - RIFT-0 Mathematical Expression Tokenizer
 * RIFT: RIFT Is a Flexible Translator
 * Component: Lexeme calculation demonstration for mathematical expressions
 * Location: examples/lexem-calculation/
 * OBINexus Computing Framework - Aegis Project
 * Collaborator: Nnamdi Michael Okpala
 * =================================================================
 */

#include "rift-0/core/tokenizer_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Mathematical expression evaluation context */
typedef struct {
    const char *expression;
    TokenizationResult tokens;
    FILE *csv_output;
    char timestamp[32];
} CalculationContext;

/* Function prototypes */
static bool initialize_calculation_context(CalculationContext *ctx, const char *expr, const char *output_file);
static void cleanup_calculation_context(CalculationContext *ctx);
static bool tokenize_mathematical_expression(CalculationContext *ctx);
static void generate_csv_output(CalculationContext *ctx);
static void print_token_analysis(CalculationContext *ctx);
static double evaluate_simple_expression(const TokenizationResult *tokens, const char *source);
static const char* get_operator_precedence_class(char op);
static void demonstrate_r_pattern_math(CalculationContext *ctx);

/* Mathematical test expressions for demonstration */
static const char* test_expressions[] = {
    "3 + 4 * 2",
    "(15 - 3) / 4",
    "2.5 * pi + sqrt(16)",
    "sin(45) + cos(30)",
    "R\"math-expr(x^2 + 2*x + 1)math-expr\"",
    "a = 42; b = nil; result = a + b",
    "matrix[i][j] = (alpha * beta) / gamma",
    "NULL == nil ? 0 : 1"
};

/**
 * Main demonstration entry point
 * Validates mathematical expression tokenization and produces CSV analysis
 */
int main(int argc, char *argv[]) {
    printf("=================================================================\n");
    printf("RIFT-0 Mathematical Expression Tokenizer Demonstration\n");
    printf("Aegis Project - Lexeme Calculation Analysis\n");
    printf("Collaborator: Nnamdi Michael Okpala\n");
    printf("=================================================================\n\n");
    
    /* Initialize tokenizer system */
    init_tokenizer_rules();
    
    const char *output_file = (argc > 1) ? argv[1] : "lexeme_analysis.csv";
    
    /* Process each test expression */
    size_t num_expressions = sizeof(test_expressions) / sizeof(test_expressions[0]);
    
    for (size_t i = 0; i < num_expressions; i++) {
        printf("Processing Expression %zu: %s\n", i + 1, test_expressions[i]);
        printf("--------------------------------------------------------\n");
        
        CalculationContext ctx;
        if (!initialize_calculation_context(&ctx, test_expressions[i], output_file)) {
            printf("Error: Failed to initialize context for expression %zu\n", i + 1);
            continue;
        }
        
        /* Perform tokenization */
        if (tokenize_mathematical_expression(&ctx)) {
            /* Generate CSV output */
            generate_csv_output(&ctx);
            
            /* Print analysis to console */
            print_token_analysis(&ctx);
            
            /* Demonstrate R pattern processing */
            if (strstr(test_expressions[i], "R\"")) {
                demonstrate_r_pattern_math(&ctx);
            }
            
            /* Attempt simple evaluation */
            double result = evaluate_simple_expression(&ctx.tokens, test_expressions[i]);
            if (!isnan(result)) {
                printf("Evaluated Result: %.6f\n", result);
            }
        } else {
            printf("Error: Tokenization failed for expression %zu\n", i + 1);
        }
        
        cleanup_calculation_context(&ctx);
        printf("\n");
    }
    
    cleanup_tokenizer_rules();
    
    printf("=================================================================\n");
    printf("Mathematical expression tokenization analysis complete.\n");
    printf("CSV output written to: %s\n", output_file);
    printf("=================================================================\n");
    
    return 0;
}

/**
 * Initialize calculation context for processing
 */
static bool initialize_calculation_context(CalculationContext *ctx, const char *expr, const char *output_file) {
    if (!ctx || !expr) return false;
    
    memset(ctx, 0, sizeof(CalculationContext));
    ctx->expression = expr;
    
    /* Generate timestamp for CSV */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(ctx->timestamp, sizeof(ctx->timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    /* Open CSV file for appending */
    ctx->csv_output = fopen(output_file, "a");
    if (!ctx->csv_output) {
        printf("Warning: Could not open CSV file %s for writing\n", output_file);
        return false;
    }
    
    /* Write CSV header if file is empty */
    fseek(ctx->csv_output, 0, SEEK_END);
    if (ftell(ctx->csv_output) == 0) {
        fprintf(ctx->csv_output, "Timestamp,Expression,Token_Index,Token_Type,Memory_Ptr,Value,Source_Text,Analysis\n");
    }
    
    return true;
}

/**
 * Cleanup calculation context
 */
static void cleanup_calculation_context(CalculationContext *ctx) {
    if (!ctx) return;
    
    if (ctx->csv_output) {
        fclose(ctx->csv_output);
    }
    
    free_tokenization_result(&ctx->tokens);
    memset(ctx, 0, sizeof(CalculationContext));
}

/**
 * Tokenize mathematical expression using RIFT-0 tokenizer
 */
static bool tokenize_mathematical_expression(CalculationContext *ctx) {
    if (!ctx || !ctx->expression) return false;
    
    size_t expr_len = strlen(ctx->expression);
    ctx->tokens = tokenize_source(ctx->expression, expr_len);
    
    if (!ctx->tokens.success) {
        printf("Tokenization failed: %s\n", 
               ctx->tokens.error_message ? ctx->tokens.error_message : "Unknown error");
        return false;
    }
    
    printf("Successfully tokenized %zu tokens\n", ctx->tokens.count);
    return true;
}

/**
 * Generate CSV output for token analysis
 */
static void generate_csv_output(CalculationContext *ctx) {
    if (!ctx || !ctx->csv_output || !ctx->tokens.success) return;
    
    for (size_t i = 0; i < ctx->tokens.count; i++) {
        const TokenTriplet *token = &ctx->tokens.tokens[i];
        
        /* Extract source text for this token */
        const char *source_start = ctx->expression + token->mem_ptr;
        size_t token_length = (token->value > 0) ? token->value : 1;
        
        /* Ensure we don't read beyond expression bounds */
        if (token->mem_ptr + token_length > strlen(ctx->expression)) {
            token_length = strlen(ctx->expression) - token->mem_ptr;
        }
        
        char source_text[64] = {0};
        if (token_length > 0 && token_length < sizeof(source_text)) {
            strncpy(source_text, source_start, token_length);
        }
        
        /* Generate analysis based on token type */
        const char *analysis = "";
        switch (token->type) {
            case TOKEN_LITERAL_NUMBER:
                analysis = "Numeric operand";
                break;
            case TOKEN_OPERATOR:
                analysis = get_operator_precedence_class(source_text[0]);
                break;
            case TOKEN_IDENTIFIER:
                analysis = "Variable/function identifier";
                break;
            case TOKEN_DELIMITER:
                analysis = "Grouping/separator";
                break;
            case TOKEN_R_PATTERN:
                analysis = "Raw mathematical expression";
                break;
            case TOKEN_NULL_KEYWORD:
                analysis = "NULL semantic (void intent)";
                break;
            case TOKEN_NIL_KEYWORD:
                analysis = "nil semantic (unallocated state)";
                break;
            default:
                analysis = "Other token";
        }
        
        /* Write CSV record */
        fprintf(ctx->csv_output, "\"%s\",\"%s\",%zu,%s,%u,%u,\"%s\",\"%s\"\n",
                ctx->timestamp,
                ctx->expression,
                i,
                token_type_to_string(token->type),
                token->mem_ptr,
                token->value,
                source_text,
                analysis);
    }
    
    fflush(ctx->csv_output);
}

/**
 * Print detailed token analysis to console
 */
static void print_token_analysis(CalculationContext *ctx) {
    if (!ctx || !ctx->tokens.success) return;
    
    printf("Token Analysis:\n");
    printf("Index | Type          | Mem_Ptr | Value | Source Text | Analysis\n");
    printf("------|---------------|---------|-------|-------------|------------------\n");
    
    for (size_t i = 0; i < ctx->tokens.count; i++) {
        const TokenTriplet *token = &ctx->tokens.tokens[i];
        
        /* Extract source text */
        const char *source_start = ctx->expression + token->mem_ptr;
        size_t token_length = (token->value > 0) ? token->value : 1;
        
        if (token->mem_ptr + token_length > strlen(ctx->expression)) {
            token_length = strlen(ctx->expression) - token->mem_ptr;
        }
        
        char source_text[32] = {0};
        if (token_length > 0 && token_length < sizeof(source_text)) {
            strncpy(source_text, source_start, token_length);
        }
        
        printf("%5zu | %-13s | %7u | %5u | %-11s | %s\n",
               i,
               token_type_to_string(token->type),
               token->mem_ptr,
               token->value,
               source_text,
               (token->type == TOKEN_OPERATOR) ? 
                   get_operator_precedence_class(source_text[0]) : "");
    }
}

/**
 * Simple mathematical expression evaluator for demonstration
 * Note: This is a simplified evaluator for demonstration purposes
 */
static double evaluate_simple_expression(const TokenizationResult *tokens, const char *source) {
    if (!tokens || !tokens->success || tokens->count == 0) {
        return NAN;
    }
    
    /* For demonstration, handle simple binary operations */
    if (tokens->count == 3) {
        /* Pattern: number operator number */
        if (tokens->tokens[0].type == TOKEN_LITERAL_NUMBER &&
            tokens->tokens[1].type == TOKEN_OPERATOR &&
            tokens->tokens[2].type == TOKEN_LITERAL_NUMBER) {
            
            /* Extract operands */
            double left = atof(source + tokens->tokens[0].mem_ptr);
            double right = atof(source + tokens->tokens[2].mem_ptr);
            char operator = *(source + tokens->tokens[1].mem_ptr);
            
            /* Perform operation */
            switch (operator) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/': return (right != 0.0) ? left / right : NAN;
                default: return NAN;
            }
        }
    }
    
    /* Handle single number */
    if (tokens->count == 1 && tokens->tokens[0].type == TOKEN_LITERAL_NUMBER) {
        return atof(source + tokens->tokens[0].mem_ptr);
    }
    
    return NAN; /* Complex expressions not supported in this demo */
}

/**
 * Get operator precedence classification for analysis
 */
static const char* get_operator_precedence_class(char op) {
    switch (op) {
        case '+': case '-':
            return "Low precedence (addition/subtraction)";
        case '*': case '/': case '%':
            return "Medium precedence (multiplication/division)";
        case '^':
            return "High precedence (exponentiation)";
        case '=':
            return "Assignment operator";
        case '<': case '>': case '!':
            return "Comparison operator";
        case '(': case ')':
            return "Grouping operator";
        default:
            return "Unknown operator";
    }
}

/**
 * Demonstrate R pattern processing for mathematical expressions
 */
static void demonstrate_r_pattern_math(CalculationContext *ctx) {
    if (!ctx || !ctx->tokens.success) return;
    
    printf("R Pattern Mathematical Expression Analysis:\n");
    
    for (size_t i = 0; i < ctx->tokens.count; i++) {
        if (ctx->tokens.tokens[i].type == TOKEN_R_PATTERN) {
            printf("  Found R pattern at index %zu\n", i);
            printf("  Memory pointer: %u\n", ctx->tokens.tokens[i].mem_ptr);
            printf("  Pattern length: %u\n", ctx->tokens.tokens[i].value);
            
            /* Extract and analyze the R pattern content */
            const char *pattern_start = ctx->expression + ctx->tokens.tokens[i].mem_ptr;
            printf("  Pattern content: \"");
            for (size_t j = 0; j < ctx->tokens.tokens[i].value && j < 50; j++) {
                putchar(pattern_start[j]);
            }
            printf("\"\n");
            printf("  Analysis: Raw mathematical expression preserved\n");
            printf("  Use case: Template processing, symbolic math, code generation\n");
        }
    }
}
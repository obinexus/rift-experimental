/**
 * @file ast_contextualization.c
 * @brief AST contextualization with state minimization and Tennis FSM validation
 */

#include "obinexus/ast_context.h"
#include "obinexus/minimization.h"
#include "obinexus/tennis_fsm.h"
#include "obinexus/semantics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    StateMinimizer* minimizer;
    TennisFSMValidator* tennis_validator;
    SemanticPreserver* semantic_preserver;
    bool zero_trust_mode;
    uint32_t confidence_threshold;
} ASTContext;

ASTContext* ast_create_context(bool zero_trust_mode) {
    ASTContext* ctx = malloc(sizeof(ASTContext));
    if (!ctx) return NULL;
    
    ctx->minimizer = state_minimizer_create();
    ctx->tennis_validator = tennis_fsm_validator_create();
    ctx->semantic_preserver = semantic_preserver_create();
    ctx->zero_trust_mode = zero_trust_mode;
    ctx->confidence_threshold = 85;
    
    return ctx;
}

bool ast_minimize_with_zero_trust(ASTContext* ctx, const char* input_file, const char* output_file) {
    // 1. Verify input artifact AuraSeal (if Zero Trust mode)
    if (ctx->zero_trust_mode) {
        if (!auraseal_verify_artifact(input_file)) {
            printf("CRITICAL: Stage 2→3 transition: AuraSeal verification FAILED\n");
            return false;
        }
    }
    
    // 2. Load parse tree
    ParseTree* parse_tree = parse_tree_load(input_file);
    if (!parse_tree) {
        printf("ERROR: Failed to load parse tree from %s\n", input_file);
        return false;
    }
    
    // 3. Apply state minimization using Tennis FSM algorithm
    MinimizationResult result = state_minimizer_apply_tennis_algorithm(ctx->minimizer, parse_tree);
    if (result.confidence < ctx->confidence_threshold) {
        printf("CRITICAL: AST minimization confidence %d below threshold %d\n", 
               result.confidence, ctx->confidence_threshold);
        parse_tree_free(parse_tree);
        return false;
    }
    
    // 4. Tennis FSM validation for equivalence
    TennisFSMResult tennis_result = tennis_fsm_validate_minimization(ctx->tennis_validator, 
                                                                    parse_tree, result.minimized_ast);
    if (!tennis_result.equivalence_proven) {
        printf("CRITICAL: Tennis FSM validation failed - minimization not equivalent\n");
        parse_tree_free(parse_tree);
        return false;
    }
    
    // 5. Semantic preservation verification
    SemanticScore semantic_score = semantic_preserver_verify(ctx->semantic_preserver, 
                                                           parse_tree, result.minimized_ast);
    if (semantic_score.preservation_score < ctx->confidence_threshold) {
        printf("CRITICAL: Semantic preservation score %d below threshold\n", semantic_score.preservation_score);
        parse_tree_free(parse_tree);
        return false;
    }
    
    // 6. Output contextualized AST
    FILE* output = fopen(output_file, "w");
    if (!output) {
        parse_tree_free(parse_tree);
        return false;
    }
    
    fprintf(output, "# RIFT Stage 3 Output - Contextualized AST with State Minimization\n");
    fprintf(output, "# Input: %s\n", input_file);
    fprintf(output, "# Zero Trust: %s\n", ctx->zero_trust_mode ? "ENABLED" : "DISABLED");
    fprintf(output, "# Minimization Confidence: %d%%\n", result.confidence);
    fprintf(output, "# Tennis FSM Validation: %s\n", tennis_result.equivalence_proven ? "PASSED" : "FAILED");
    fprintf(output, "# Semantic Preservation: %d%%\n\n", semantic_score.preservation_score);
    
    // Output minimized AST structure
    ast_print_contextualized(result.minimized_ast, output);
    
    // Add Tennis FSM validation metadata
    fprintf(output, "\n# Tennis FSM Validation Metadata\n");
    fprintf(output, "# Original States: %d\n", tennis_result.original_states);
    fprintf(output, "# Minimized States: %d\n", tennis_result.minimized_states);
    fprintf(output, "# State Reduction: %.2f%%\n", tennis_result.reduction_percentage);
    fprintf(output, "# Equivalence Verification: %s\n", tennis_result.equivalence_proven ? "PROVEN" : "FAILED");
    
    fclose(output);
    parse_tree_free(parse_tree);
    
    // 7. Apply AuraSeal to output (if Zero Trust mode)
    if (ctx->zero_trust_mode) {
        if (!auraseal_sign_artifact(output_file, "STAGE_3_AST_MINIMIZED")) {
            printf("CRITICAL: Failed to apply AuraSeal to minimized AST\n");
            return false;
        }
    }
    
    return true;
}

bool ast_process_file(ASTContext* ctx, const char* input_file, const char* output_file) {
    return ast_minimize_with_zero_trust(ctx, input_file, output_file);
}

void ast_free_context(ASTContext* ctx) {
    if (!ctx) return;
    
    state_minimizer_free(ctx->minimizer);
    tennis_fsm_validator_free(ctx->tennis_validator);
    semantic_preserver_free(ctx->semantic_preserver);
    free(ctx);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.rift.1> <output.rift.3> [--zero-trust]\n", argv[0]);
        return 1;
    }
    
    bool zero_trust = (argc > 3 && strcmp(argv[3], "--zero-trust") == 0);
    
    ASTContext* ctx = ast_create_context(zero_trust);
    if (!ctx) {
        fprintf(stderr, "Failed to create AST context\n");
        return 1;
    }
    
    bool success = ast_process_file(ctx, argv[1], argv[2]);
    ast_free_context(ctx);
    
    if (success) {
        printf("Stage 3 AST contextualization completed: %s\n", argv[2]);
        printf("Tennis FSM validation: PASSED\n");
        printf("Zero Trust enforcement: %s\n", zero_trust ? "ENABLED" : "DISABLED");
        return 0;
    } else {
        fprintf(stderr, "Stage 3 AST contextualization failed\n");
        return 1;
    }
}

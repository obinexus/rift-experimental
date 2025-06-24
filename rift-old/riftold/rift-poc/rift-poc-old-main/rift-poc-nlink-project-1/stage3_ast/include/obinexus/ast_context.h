#ifndef AST_CONTEXT_H
#define AST_CONTEXT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct ASTContext ASTContext;
typedef struct ParseTree ParseTree;
typedef struct AST AST;

ASTContext* ast_create_context(bool zero_trust_mode);
bool ast_process_file(ASTContext* ctx, const char* input_file, const char* output_file);
void ast_free_context(ASTContext* ctx);

// Zero Trust integration
bool auraseal_verify_artifact(const char* file_path);
bool auraseal_sign_artifact(const char* file_path, const char* stage_id);

// Parse tree operations
ParseTree* parse_tree_load(const char* file_path);
void parse_tree_free(ParseTree* tree);

// AST operations
void ast_print_contextualized(AST* ast, FILE* output);

#endif

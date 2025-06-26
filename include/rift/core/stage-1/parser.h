/*
 * rift/include/rift/core/stage-1/parser.h
 * RIFT Stage 1: Parser Engine Header
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#ifndef RIFT_CORE_STAGE_1_PARSER_H
#define RIFT_CORE_STAGE_1_PARSER_H

#include "rift/core/common.h"
#include "rift/core/stage-0/tokenizer.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Parser Framework Version
#define RIFT_PARSER_VERSION_MAJOR 1
#define RIFT_PARSER_VERSION_MINOR 0
#define RIFT_PARSER_VERSION_PATCH 0

// AST Node Types
typedef enum {
    AST_NODE_PROGRAM,
    AST_NODE_STATEMENT,
    AST_NODE_EXPRESSION,
    AST_NODE_DECLARATION,
    AST_NODE_ASSIGNMENT,
    AST_NODE_BINARY_OP,
    AST_NODE_UNARY_OP,
    AST_NODE_LITERAL,
    AST_NODE_IDENTIFIER,
    AST_NODE_FUNCTION_CALL,
    AST_NODE_BLOCK,
    AST_NODE_IF_STATEMENT,
    AST_NODE_WHILE_LOOP,
    AST_NODE_FOR_LOOP,
    AST_NODE_RETURN_STATEMENT
} rift_ast_node_type_t;

// AST Node Structure
typedef struct rift_ast_node {
    rift_ast_node_type_t type;
    char value[256];
    size_t matched_state;
    rift_source_location_t location;
    struct rift_ast_node** children;
    size_t child_count;
    size_t child_capacity;
    size_t complexity_score;
} rift_ast_node_t;

// Parser State Management
typedef struct {
    const rift_token_t* tokens;
    size_t token_count;
    size_t current_position;
    rift_ast_node_t* root;
    bool aegis_validation_enabled;
    rift_error_context_t error_context;
} rift_parser_state_t;

/*
 * Core Parser Functions
 */

/**
 * rift_parser_init - Initialize parser with token input
 * @tokens: Token array from tokenizer
 * @token_count: Number of tokens
 * @state: Parser state to initialize
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_parser_init(const rift_token_t* tokens, size_t token_count,
                     rift_parser_state_t* state);

/**
 * rift_parser_process - Main parsing function
 * @state: Initialized parser state
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_parser_process(rift_parser_state_t* state);

/**
 * rift_parser_cleanup - Resource cleanup
 * @state: Parser state to cleanup
 */
void rift_parser_cleanup(rift_parser_state_t* state);

/**
 * rift_parser_get_ast - Get parsed AST root
 * @state: Parser state
 * 
 * Returns: Pointer to AST root node
 */
const rift_ast_node_t* rift_parser_get_ast(const rift_parser_state_t* state);

/*
 * AST Node Management Functions
 */

/**
 * rift_ast_node_create - Create new AST node
 * @type: Node type
 * @value: Node value (optional)
 * 
 * Returns: Pointer to new AST node, or NULL on failure
 */
rift_ast_node_t* rift_ast_node_create(rift_ast_node_type_t type, const char* value);

/**
 * rift_ast_node_add_child - Add child node
 * @parent: Parent node
 * @child: Child node to add
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_ast_node_add_child(rift_ast_node_t* parent, rift_ast_node_t* child);

/**
 * rift_ast_node_destroy - Destroy AST node and children
 * @node: Node to destroy
 */
void rift_ast_node_destroy(rift_ast_node_t* node);

/*
 * Parsing Strategy Functions
 */
int rift_parse_program(rift_parser_state_t* state);
int rift_parse_statement(rift_parser_state_t* state, rift_ast_node_t** result);
int rift_parse_expression(rift_parser_state_t* state, rift_ast_node_t** result);
int rift_parse_declaration(rift_parser_state_t* state, rift_ast_node_t** result);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_1_PARSER_H */

/*
 * rift/src/core/stage-1/parser.c
 * RIFT Stage 1: Parser Engine Implementation
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rift/core/stage-1/parser.h"
#include "rift/core/common.h"
#include "rift/governance/policy.h"

// AEGIS Parser Constants
#define RIFT_MAX_AST_CHILDREN 32
#define RIFT_MAX_PARSE_DEPTH 64

// Forward declarations
static rift_token_t current_token(const rift_parser_state_t* state);
static rift_token_t peek_token(const rift_parser_state_t* state, size_t offset);
static int advance_parser(rift_parser_state_t* state);
static bool match_token_type(const rift_parser_state_t* state, rift_token_type_t type);
static int expect_token_type(rift_parser_state_t* state, rift_token_type_t type);

/*
 * rift_parser_init - Initialize parser with AEGIS compliance
 */
int rift_parser_init(const rift_token_t* tokens, size_t token_count,
                     rift_parser_state_t* state) {
    if (!tokens || !state || token_count == 0) {
        return RIFT_ERROR_INVALID_ARGUMENT;
    }

    // Initialize parser state
    state->tokens = tokens;
    state->token_count = token_count;
    state->current_position = 0;
    state->root = NULL;
    state->aegis_validation_enabled = true;

    // Initialize error context
    rift_error_context_init(&state->error_context, RIFT_SUCCESS, 
                           "Parser initialized", NULL, 
                           "rift_parser_init", "stage-1-parser");

    return RIFT_SUCCESS;
}

/*
 * rift_parser_process - Main parsing processing function
 */
int rift_parser_process(rift_parser_state_t* state) {
    if (!state || !state->tokens) {
        return RIFT_ERROR_INVALID_STATE;
    }

    // Create root program node
    state->root = rift_ast_node_create(AST_NODE_PROGRAM, "program");
    if (!state->root) {
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }

    // Parse program
    int result = rift_parse_program(state);
    if (result != RIFT_SUCCESS) {
        rift_ast_node_destroy(state->root);
        state->root = NULL;
        return result;
    }

    // AEGIS governance validation
    if (state->aegis_validation_enabled) {
        result = rift_governance_validate_ast_tree(state->root);
        if (result != RIFT_SUCCESS) {
            return RIFT_ERROR_GOVERNANCE_VIOLATION;
        }
    }

    return RIFT_SUCCESS;
}

/*
 * rift_parser_cleanup - Resource cleanup
 */
void rift_parser_cleanup(rift_parser_state_t* state) {
    if (state && state->root) {
        rift_ast_node_destroy(state->root);
        state->root = NULL;
    }
}

/*
 * rift_parser_get_ast - Get parsed AST root
 */
const rift_ast_node_t* rift_parser_get_ast(const rift_parser_state_t* state) {
    return state ? state->root : NULL;
}

/*
 * AST Node Management Functions
 */

/*
 * rift_ast_node_create - Create new AST node
 */
rift_ast_node_t* rift_ast_node_create(rift_ast_node_type_t type, const char* value) {
    rift_ast_node_t* node = malloc(sizeof(rift_ast_node_t));
    if (!node) {
        return NULL;
    }

    node->type = type;
    node->matched_state = 0;
    node->child_count = 0;
    node->child_capacity = RIFT_MAX_AST_CHILDREN;
    node->complexity_score = 1;

    // Initialize value
    if (value) {
        strncpy(node->value, value, sizeof(node->value) - 1);
        node->value[sizeof(node->value) - 1] = '\0';
    } else {
        node->value[0] = '\0';
    }

    // Allocate children array
    node->children = malloc(sizeof(rift_ast_node_t*) * RIFT_MAX_AST_CHILDREN);
    if (!node->children) {
        free(node);
        return NULL;
    }

    return node;
}

/*
 * rift_ast_node_add_child - Add child node
 */
int rift_ast_node_add_child(rift_ast_node_t* parent, rift_ast_node_t* child) {
    if (!parent || !child) {
        return RIFT_ERROR_INVALID_ARGUMENT;
    }

    if (parent->child_count >= parent->child_capacity) {
        return RIFT_ERROR_BUFFER_OVERFLOW;
    }

    parent->children[parent->child_count] = child;
    parent->child_count++;
    parent->complexity_score += child->complexity_score;

    return RIFT_SUCCESS;
}

/*
 * rift_ast_node_destroy - Destroy AST node and children
 */
void rift_ast_node_destroy(rift_ast_node_t* node) {
    if (!node) {
        return;
    }

    // Destroy all children recursively
    for (size_t i = 0; i < node->child_count; i++) {
        rift_ast_node_destroy(node->children[i]);
    }

    // Free children array and node
    free(node->children);
    free(node);
}

/*
 * Parsing Strategy Functions
 */

/*
 * rift_parse_program - Parse top-level program
 */
int rift_parse_program(rift_parser_state_t* state) {
    while (state->current_position < state->token_count) {
        rift_token_t token = current_token(state);
        
        // Skip EOF token
        if (token.type == TOKEN_EOF) {
            break;
        }

        rift_ast_node_t* statement = NULL;
        int result = rift_parse_statement(state, &statement);
        if (result != RIFT_SUCCESS) {
            return result;
        }

        if (statement) {
            result = rift_ast_node_add_child(state->root, statement);
            if (result != RIFT_SUCCESS) {
                rift_ast_node_destroy(statement);
                return result;
            }
        }
    }

    return RIFT_SUCCESS;
}

/*
 * rift_parse_statement - Parse statement
 */
int rift_parse_statement(rift_parser_state_t* state, rift_ast_node_t** result) {
    rift_token_t token = current_token(state);
    
    switch (token.type) {
        case TOKEN_KEYWORD:
            if (strcmp(token.value, "let") == 0 || strcmp(token.value, "const") == 0) {
                return rift_parse_declaration(state, result);
            }
            break;
            
        case TOKEN_IDENTIFIER:
            return rift_parse_expression(state, result);
            
        default:
            // Skip unknown tokens
            advance_parser(state);
            *result = NULL;
            return RIFT_SUCCESS;
    }

    advance_parser(state);
    *result = NULL;
    return RIFT_SUCCESS;
}

/*
 * rift_parse_expression - Parse expression
 */
int rift_parse_expression(rift_parser_state_t* state, rift_ast_node_t** result) {
    rift_token_t token = current_token(state);
    
    // Simple expression parsing (identifier or literal)
    if (token.type == TOKEN_IDENTIFIER || 
        token.type == TOKEN_LITERAL_INTEGER ||
        token.type == TOKEN_LITERAL_FLOAT ||
        token.type == TOKEN_LITERAL_STRING) {
        
        rift_ast_node_t* node = rift_ast_node_create(AST_NODE_EXPRESSION, token.value);
        if (!node) {
            return RIFT_ERROR_MEMORY_ALLOCATION;
        }
        
        node->location = (rift_source_location_t){
            .filename = "",
            .line_number = token.line_number,
            .column_number = token.column_number,
            .character_offset = 0
        };
        
        advance_parser(state);
        *result = node;
        return RIFT_SUCCESS;
    }

    // Skip unexpected tokens
    advance_parser(state);
    *result = NULL;
    return RIFT_SUCCESS;
}

/*
 * rift_parse_declaration - Parse declaration
 */
int rift_parse_declaration(rift_parser_state_t* state, rift_ast_node_t** result) {
    rift_token_t keyword_token = current_token(state);
    
    // Create declaration node
    rift_ast_node_t* decl_node = rift_ast_node_create(AST_NODE_DECLARATION, keyword_token.value);
    if (!decl_node) {
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    advance_parser(state); // Skip keyword
    
    // Expect identifier
    if (expect_token_type(state, TOKEN_IDENTIFIER) != RIFT_SUCCESS) {
        rift_ast_node_destroy(decl_node);
        return RIFT_ERROR_SYNTAX_ERROR;
    }
    
    rift_token_t id_token = current_token(state);
    rift_ast_node_t* id_node = rift_ast_node_create(AST_NODE_IDENTIFIER, id_token.value);
    if (!id_node) {
        rift_ast_node_destroy(decl_node);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    rift_ast_node_add_child(decl_node, id_node);
    advance_parser(state);
    
    // Optional assignment
    if (match_token_type(state, TOKEN_OPERATOR)) {
        rift_token_t op_token = current_token(state);
        if (strcmp(op_token.value, "=") == 0) {
            advance_parser(state); // Skip '='
            
            rift_ast_node_t* expr_node = NULL;
            int result = rift_parse_expression(state, &expr_node);
            if (result == RIFT_SUCCESS && expr_node) {
                rift_ast_node_add_child(decl_node, expr_node);
            }
        }
    }
    
    *result = decl_node;
    return RIFT_SUCCESS;
}

/*
 * Helper Functions
 */

static rift_token_t current_token(const rift_parser_state_t* state) {
    if (state->current_position < state->token_count) {
        return state->tokens[state->current_position];
    }
    
    // Return EOF token if at end
    rift_token_t eof_token = {0};
    eof_token.type = TOKEN_EOF;
    return eof_token;
}

static rift_token_t peek_token(const rift_parser_state_t* state, size_t offset) {
    size_t peek_pos = state->current_position + offset;
    if (peek_pos < state->token_count) {
        return state->tokens[peek_pos];
    }
    
    rift_token_t eof_token = {0};
    eof_token.type = TOKEN_EOF;
    return eof_token;
}

static int advance_parser(rift_parser_state_t* state) {
    if (state->current_position < state->token_count) {
        state->current_position++;
        return RIFT_SUCCESS;
    }
    return RIFT_ERROR_END_OF_INPUT;
}

static bool match_token_type(const rift_parser_state_t* state, rift_token_type_t type) {
    rift_token_t token = current_token(state);
    return token.type == type;
}

static int expect_token_type(rift_parser_state_t* state, rift_token_type_t type) {
    if (match_token_type(state, type)) {
        return RIFT_SUCCESS;
    }
    return RIFT_ERROR_UNEXPECTED_TOKEN;
}

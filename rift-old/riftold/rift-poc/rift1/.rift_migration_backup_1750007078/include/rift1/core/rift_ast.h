#ifndef RIFT1_AST_H
#define RIFT1_AST_H

#include "rift_errors.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * RIFT1 AST Definitions
 * OBINexus AEGIS Framework - Stage 1
 * 
 * Abstract Syntax Tree structures and operations
 */

// Forward declarations
typedef struct RiftASTNode RiftASTNode;

// AST Node Types - PROPER ENUM DEFINITION
typedef enum {
    RIFT_AST_NODE_UNKNOWN = 0,
    RIFT_AST_NODE_LITERAL = 1,
    RIFT_AST_NODE_PATTERN = 2,
    RIFT_AST_NODE_EXPRESSION = 3,
    RIFT_AST_NODE_SEQUENCE = 4,
    RIFT_AST_NODE_CHOICE = 5
} RiftASTNodeType;

// AST Node Structure - COMPLETE DEFINITION
struct RiftASTNode {
    RiftASTNodeType type;
    char* value;
    bool matched_state;              // AEGIS: AST minimization support
    struct RiftASTNode* parent;
    struct RiftASTNode** children;
    size_t children_count;
    size_t children_capacity;
};

// Function declarations - FIXED SIGNATURES
RiftASTNode* rift_ast_node_create(RiftASTNodeType type, const char* value);
RiftResult rift_ast_node_add_child(RiftASTNode* parent, RiftASTNode* child);
RiftResult rift_ast_node_destroy(RiftASTNode* node);
RiftResult rift_ast_optimize(RiftASTNode* root);

// AST utility functions
const char* rift_ast_node_type_to_string(RiftASTNodeType type);
bool rift_ast_node_is_leaf(const RiftASTNode* node);
size_t rift_ast_node_depth(const RiftASTNode* node);

#endif // RIFT1_AST_H

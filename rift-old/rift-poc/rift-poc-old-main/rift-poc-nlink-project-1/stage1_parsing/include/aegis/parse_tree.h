#ifndef AEGIS_PARSE_TREE_H
#define AEGIS_PARSE_TREE_H

#include <stdio.h>

typedef struct ParseNode ParseNode;
typedef struct ParseTreeBuilder ParseTreeBuilder;

ParseTreeBuilder* parse_tree_builder_create(void);
void parse_tree_builder_free(ParseTreeBuilder* builder);

ParseNode* parse_tree_create_node(ParseTreeBuilder* builder, const char* value);
ParseNode* parse_tree_create_leaf(ParseTreeBuilder* builder, const char* value);
void parse_tree_add_child(ParseNode* parent, ParseNode* child);
void parse_tree_print(ParseNode* node, FILE* output, int depth);

#endif

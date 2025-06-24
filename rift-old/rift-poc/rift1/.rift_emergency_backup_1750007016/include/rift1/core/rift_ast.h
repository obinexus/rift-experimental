#ifndef RIFT_AST_H
#define RIFT_AST_H

void rift_ast_node_destroy(RiftASTNode* node) ;
RiftResult rift_ast_node_add_child(RiftASTNode* parent, RiftASTNode* child) ;
RiftResult rift_ast_optimize(RiftASTNode* root) ;

#endif // RIFT_AST_H

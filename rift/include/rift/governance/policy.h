#ifndef RIFT_GOVERNANCE_POLICY_H
#define RIFT_GOVERNANCE_POLICY_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for governance functions
struct rift_token;
struct rift_ast_node;

// Governance validation function stubs
int rift_governance_validate_token(const struct rift_token* token);
int rift_governance_validate_ast_tree(const struct rift_ast_node* root);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_GOVERNANCE_POLICY_H */

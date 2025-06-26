/*
 * rift/include/rift/governance/policy.h
 * RIFT Governance Policy Framework Header
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#ifndef RIFT_GOVERNANCE_POLICY_H
#define RIFT_GOVERNANCE_POLICY_H

#include "rift/core/common.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Governance Framework Version
#define RIFT_GOVERNANCE_VERSION_MAJOR 1
#define RIFT_GOVERNANCE_VERSION_MINOR 0
#define RIFT_GOVERNANCE_VERSION_PATCH 0

// Governance Policy Constants
#define RIFT_MAX_POLICY_NAME_LENGTH 128
#define RIFT_MAX_POLICY_DESCRIPTION_LENGTH 512
#define RIFT_MAX_GOVERNANCE_RULES 256
#define RIFT_MAX_COMPLIANCE_CHECKS 128

// Forward Declarations
typedef struct rift_token rift_token_t;
typedef struct rift_ast_node rift_ast_node_t;

// Governance Policy Types
typedef enum {
    RIFT_POLICY_TYPE_SECURITY,
    RIFT_POLICY_TYPE_MEMORY_SAFETY,
    RIFT_POLICY_TYPE_TYPE_SAFETY,
    RIFT_POLICY_TYPE_PERFORMANCE,
    RIFT_POLICY_TYPE_COMPLIANCE,
    RIFT_POLICY_TYPE_AUDIT,
    RIFT_POLICY_TYPE_ZERO_TRUST,
    RIFT_POLICY_TYPE_VALIDATION
} rift_policy_type_t;

// Governance Severity Levels
typedef enum {
    RIFT_SEVERITY_INFO = 0,
    RIFT_SEVERITY_LOW = 1,
    RIFT_SEVERITY_MEDIUM = 2,
    RIFT_SEVERITY_HIGH = 3,
    RIFT_SEVERITY_CRITICAL = 4,
    RIFT_SEVERITY_FATAL = 5
} rift_governance_severity_t;

// Governance Rule Structure
typedef struct {
    char name[RIFT_MAX_POLICY_NAME_LENGTH];
    char description[RIFT_MAX_POLICY_DESCRIPTION_LENGTH];
    rift_policy_type_t policy_type;
    rift_governance_severity_t severity;
    bool is_enabled;
    bool is_mandatory;
    int priority;
    uint64_t rule_id;
} rift_governance_rule_t;

// Governance Context
typedef struct {
    rift_governance_rule_t* rules;
    size_t rule_count;
    size_t rule_capacity;
    bool zero_trust_enabled;
    bool audit_enabled;
    bool strict_mode;
    char configuration_file[RIFT_MAX_PATH_LENGTH];
} rift_governance_context_t;

// Governance Violation Report
typedef struct {
    uint64_t violation_id;
    uint64_t rule_id;
    rift_governance_severity_t severity;
    char violation_message[RIFT_MAX_ERROR_MESSAGE_LENGTH];
    rift_source_location_t location;
    uint64_t timestamp;
    bool is_resolved;
} rift_governance_violation_t;

/*
 * Core Governance Functions
 */

/**
 * rift_governance_init - Initialize governance framework
 * @context: Governance context to initialize
 * @config_file: Path to governance configuration file (optional)
 * 
 * Initializes the AEGIS governance framework with default or
 * configuration-specified policies and rules.
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_governance_init(rift_governance_context_t* context, 
                        const char* config_file);

/**
 * rift_governance_cleanup - Cleanup governance resources
 * @context: Governance context to cleanup
 */
void rift_governance_cleanup(rift_governance_context_t* context);

/**
 * rift_governance_load_config - Load governance configuration
 * @context: Governance context
 * @config_file: Path to configuration file
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_governance_load_config(rift_governance_context_t* context,
                               const char* config_file);

/*
 * Token-Level Governance Functions
 */

/**
 * rift_governance_validate_token - Validate token against governance policies
 * @token: Token to validate
 * 
 * Validates a single token against all applicable governance rules.
 * This function is called from the tokenizer during token generation.
 * 
 * Returns: RIFT_SUCCESS if token is compliant, error code if violation detected
 */
int rift_governance_validate_token(const rift_token_t* token);

/**
 * rift_governance_validate_token_sequence - Validate token sequence
 * @tokens: Array of tokens to validate
 * @token_count: Number of tokens in array
 * 
 * Validates sequences of tokens for pattern-based governance rules.
 * 
 * Returns: RIFT_SUCCESS if sequence is compliant, error code if violations detected
 */
int rift_governance_validate_token_sequence(const rift_token_t* tokens, 
                                           size_t token_count);

/*
 * AST-Level Governance Functions
 */

/**
 * rift_governance_validate_ast_node - Validate AST node
 * @node: AST node to validate
 * 
 * Validates individual AST nodes against structural governance policies.
 * 
 * Returns: RIFT_SUCCESS if node is compliant, error code if violation detected
 */
int rift_governance_validate_ast_node(const rift_ast_node_t* node);

/**
 * rift_governance_validate_ast_tree - Validate complete AST
 * @root: Root node of AST to validate
 * 
 * Performs comprehensive governance validation of entire AST structure.
 * 
 * Returns: RIFT_SUCCESS if AST is compliant, error code if violations detected
 */
int rift_governance_validate_ast_tree(const rift_ast_node_t* root);

/*
 * Memory Safety Governance
 */

/**
 * rift_governance_validate_memory_allocation - Validate memory allocation
 * @ptr: Allocated memory pointer
 * @size: Size of allocation
 * @alignment: Memory alignment requirement
 * @allocator_name: Name of allocator for audit trail
 * 
 * Validates memory allocations against governance policies for
 * memory safety and resource management.
 * 
 * Returns: RIFT_SUCCESS if allocation is compliant, error code otherwise
 */
int rift_governance_validate_memory_allocation(void* ptr, size_t size,
                                             size_t alignment,
                                             const char* allocator_name);

/**
 * rift_governance_validate_memory_access - Validate memory access
 * @ptr: Memory pointer being accessed
 * @size: Size of access
 * @access_type: Type of access (read/write)
 * 
 * Validates memory access operations for bounds checking and
 * memory safety compliance.
 * 
 * Returns: RIFT_SUCCESS if access is safe, error code otherwise
 */
int rift_governance_validate_memory_access(void* ptr, size_t size, 
                                         const char* access_type);

/*
 * Security Governance Functions
 */

/**
 * rift_governance_validate_input - Validate external input
 * @input: Input data to validate
 * @input_size: Size of input data
 * @input_type: Type/format of input
 * 
 * Validates external input against security governance policies
 * to prevent injection attacks and malformed input processing.
 * 
 * Returns: RIFT_SUCCESS if input is safe, error code if security violation
 */
int rift_governance_validate_input(const void* input, size_t input_size,
                                  const char* input_type);

/**
 * rift_governance_validate_output - Validate output generation
 * @output: Output data to validate
 * @output_size: Size of output data
 * @output_type: Type/format of output
 * 
 * Validates output generation for information leakage prevention
 * and compliance with security policies.
 * 
 * Returns: RIFT_SUCCESS if output is secure, error code if violation
 */
int rift_governance_validate_output(const void* output, size_t output_size,
                                   const char* output_type);

/*
 * Compliance and Audit Functions
 */

/**
 * rift_governance_audit_operation - Audit operation for compliance
 * @operation_name: Name of operation being audited
 * @parameters: Operation parameters (optional)
 * @result: Operation result
 * 
 * Records operation in audit trail for compliance and forensic analysis.
 * 
 * Returns: RIFT_SUCCESS on successful audit recording
 */
int rift_governance_audit_operation(const char* operation_name,
                                   const char* parameters,
                                   int result);

/**
 * rift_governance_generate_compliance_report - Generate compliance report
 * @context: Governance context
 * @output_file: File path for compliance report
 * 
 * Generates comprehensive compliance report for audit purposes.
 * 
 * Returns: RIFT_SUCCESS on successful report generation
 */
int rift_governance_generate_compliance_report(const rift_governance_context_t* context,
                                              const char* output_file);

/*
 * Rule Management Functions
 */

/**
 * rift_governance_add_rule - Add governance rule
 * @context: Governance context
 * @rule: Governance rule to add
 * 
 * Adds a new governance rule to the active policy set.
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_governance_add_rule(rift_governance_context_t* context,
                            const rift_governance_rule_t* rule);

/**
 * rift_governance_remove_rule - Remove governance rule
 * @context: Governance context
 * @rule_id: ID of rule to remove
 * 
 * Removes a governance rule from the active policy set.
 * 
 * Returns: RIFT_SUCCESS on success, error code if rule not found
 */
int rift_governance_remove_rule(rift_governance_context_t* context,
                               uint64_t rule_id);

/**
 * rift_governance_enable_rule - Enable governance rule
 * @context: Governance context
 * @rule_id: ID of rule to enable
 * 
 * Enables a previously disabled governance rule.
 * 
 * Returns: RIFT_SUCCESS on success, error code if rule not found
 */
int rift_governance_enable_rule(rift_governance_context_t* context,
                               uint64_t rule_id);

/**
 * rift_governance_disable_rule - Disable governance rule
 * @context: Governance context
 * @rule_id: ID of rule to disable
 * 
 * Disables an active governance rule (only if not mandatory).
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_governance_disable_rule(rift_governance_context_t* context,
                                uint64_t rule_id);

/*
 * Default Governance Policies
 */

/**
 * rift_governance_load_default_policies - Load default AEGIS policies
 * @context: Governance context
 * 
 * Loads the standard set of AEGIS governance policies that implement
 * the OBINexus Computing Framework requirements.
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_governance_load_default_policies(rift_governance_context_t* context);

/**
 * rift_governance_load_zero_trust_policies - Load Zero Trust policies
 * @context: Governance context
 * 
 * Loads enhanced Zero Trust governance policies for high-security
 * environments.
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int rift_governance_load_zero_trust_policies(rift_governance_context_t* context);

/*
 * Utility Functions
 */

/**
 * rift_governance_severity_to_string - Convert severity to string
 * @severity: Governance severity level
 * 
 * Returns: String representation of severity level
 */
const char* rift_governance_severity_to_string(rift_governance_severity_t severity);

/**
 * rift_governance_policy_type_to_string - Convert policy type to string
 * @policy_type: Governance policy type
 * 
 * Returns: String representation of policy type
 */
const char* rift_governance_policy_type_to_string(rift_policy_type_t policy_type);

/**
 * rift_governance_print_violation - Print governance violation
 * @violation: Violation to print
 * @output: File pointer for output
 */
void rift_governance_print_violation(const rift_governance_violation_t* violation,
                                    FILE* output);

/**
 * rift_governance_get_version - Get governance framework version
 * @major: Pointer to store major version
 * @minor: Pointer to store minor version
 * @patch: Pointer to store patch version
 */
void rift_governance_get_version(int* major, int* minor, int* patch);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_GOVERNANCE_POLICY_H */

/*
 * =================================================================
 * AEGIS Governance Framework Interface
 * OBINexus Computing Framework - Zero Trust Policy Implementation
 * Technical Architecture: Nnamdi Okpala's Systematic Methodology
 * =================================================================
 */

#ifndef RIFT_GOVERNANCE_H
#define RIFT_GOVERNANCE_H

#include <stdint.h>
#include <stdbool.h>
#include <rift/core/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// =================================================================
// AEGIS GOVERNANCE CORE STRUCTURES
// =================================================================

/**
 * AEGIS governance policy configuration
 * Enforces systematic waterfall methodology compliance
 */
typedef struct {
    // Core governance parameters
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    
    // Memory management policy
    uint32_t memory_alignment_bits;
    uint32_t quantum_fallback_qubits;
    bool memory_safety_strict;
    
    // Token validation policy
    bool token_type_validation_required;
    bool token_value_verification_required;
    bool token_memory_alignment_required;
    
    // Zero trust enforcement
    bool zero_trust_enabled;
    bool span_enforcement;
    bool governance_checks;
    uint32_t interference_tolerance;
    
    // Build and compilation policy
    bool artifact_validation_required;
    bool documentation_mandatory;
    double test_coverage_minimum;
    
    // Security configuration
    bool signature_verification_required;
    bool checksum_validation_required;
    bool supply_chain_validation;
} rift_governance_policy_t;

/**
 * AEGIS governance runtime state
 * Maintains validation state and policy enforcement context
 */
typedef struct {
    rift_governance_policy_t policy;
    
    // Runtime validation state
    bool initialized;
    bool policy_loaded;
    bool validation_passed;
    
    // Configuration file paths
    char *config_file_path;
    char *package_config_path;
    
    // Validation results cache
    bool memory_alignment_validated;
    bool token_schema_validated;
    bool zero_trust_validated;
    bool build_policy_validated;
    
    // Performance metrics
    uint64_t validation_start_time;
    uint64_t validation_duration_ms;
    uint32_t validation_checks_performed;
    
    // Error context
    rift_result_t last_validation_error;
    char last_error_message[256];
} rift_governance_t;

/**
 * Token validation context for AEGIS triplet enforcement
 * Ensures token_type, token_value, token_memory compliance
 */
typedef struct {
    // Token identification
    uint32_t token_id;
    uint32_t token_type;
    const char *token_value;
    
    // Memory alignment properties
    uint32_t memory_alignment;
    uint32_t memory_size;
    void *memory_address;
    
    // Validation state
    bool type_validated;
    bool value_validated;
    bool memory_validated;
    
    // Checksum and integrity
    uint8_t checksum_sha256[32];
    bool checksum_computed;
    bool integrity_verified;
} rift_token_validation_context_t;

// =================================================================
// AEGIS GOVERNANCE INITIALIZATION
// =================================================================

/**
 * Initialize AEGIS governance framework
 * Loads configuration and establishes policy enforcement context
 * 
 * @param governance Governance state structure to initialize
 * @param config_file Path to .riftrc governance configuration file
 * @return RIFT_SUCCESS on successful initialization, error code on failure
 */
rift_result_t rift_governance_init(rift_governance_t *governance,
                                   const char *config_file);

/**
 * Load governance policy from configuration file
 * Parses .riftrc and establishes systematic policy parameters
 * 
 * @param governance Governance state structure
 * @param config_file Path to configuration file
 * @return RIFT_SUCCESS on successful load, error code on failure
 */
rift_result_t rift_governance_load_policy(rift_governance_t *governance,
                                          const char *config_file);

/**
 * Validate governance configuration integrity
 * Ensures configuration file syntax and semantic correctness
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if configuration valid, error code on failure
 */
rift_result_t rift_governance_validate_config(rift_governance_t *governance);

/**
 * Cleanup governance framework resources
 * Systematic resource deallocation and state cleanup
 * 
 * @param governance Governance state structure to cleanup
 */
void rift_governance_cleanup(rift_governance_t *governance);

// =================================================================
// MEMORY ALIGNMENT VALIDATION
// =================================================================

/**
 * Validate memory alignment compliance
 * Enforces 4096-bit classical memory alignment requirements
 * 
 * @param governance Governance state structure
 * @param required_alignment Required alignment in bits
 * @return RIFT_SUCCESS if alignment compliant, error code on failure
 */
rift_result_t rift_governance_validate_memory_alignment(rift_governance_t *governance,
                                                        uint32_t required_alignment);

/**
 * Validate memory safety enforcement
 * Ensures systematic memory safety policy compliance
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if memory safety enforced, error code on failure
 */
rift_result_t rift_governance_validate_memory_safety(rift_governance_t *governance);

/**
 * Get effective memory alignment configuration
 * Returns current memory alignment policy with fallback handling
 * 
 * @param governance Governance state structure
 * @param classical_bits Output parameter for classical alignment
 * @param quantum_qubits Output parameter for quantum fallback
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_governance_get_memory_config(rift_governance_t *governance,
                                                uint32_t *classical_bits,
                                                uint32_t *quantum_qubits);

// =================================================================
// TOKEN SCHEMA VALIDATION
// =================================================================

/**
 * Initialize token validation context
 * Prepares context for systematic token triplet validation
 * 
 * @param context Token validation context to initialize
 * @param token_id Unique token identifier
 * @param token_type Token type classification
 * @param token_value Token value string
 * @return RIFT_SUCCESS on successful initialization, error code on failure
 */
rift_result_t rift_token_validation_init(rift_token_validation_context_t *context,
                                         uint32_t token_id,
                                         uint32_t token_type,
                                         const char *token_value);

/**
 * Validate complete token schema compliance
 * Enforces token_type, token_value, token_memory triplet requirements
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if token schema compliant, error code on failure
 */
rift_result_t rift_governance_validate_token_schema(rift_governance_t *governance);

/**
 * Validate individual token against AEGIS requirements
 * Comprehensive validation of token triplet properties
 * 
 * @param governance Governance state structure
 * @param context Token validation context
 * @return RIFT_SUCCESS if token valid, error code on failure
 */
rift_result_t rift_governance_validate_token(rift_governance_t *governance,
                                             rift_token_validation_context_t *context);

/**
 * Compute token integrity checksum
 * Generates SHA-256 checksum for token validation
 * 
 * @param context Token validation context
 * @return RIFT_SUCCESS if checksum computed, error code on failure
 */
rift_result_t rift_token_compute_checksum(rift_token_validation_context_t *context);

// =================================================================
// ZERO TRUST POLICY ENFORCEMENT
// =================================================================

/**
 * Validate zero trust policy compliance
 * Ensures systematic zero trust methodology enforcement
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if zero trust compliant, error code on failure
 */
rift_result_t rift_governance_validate_zero_trust(rift_governance_t *governance);

/**
 * Validate span enforcement policy
 * Ensures systematic span isolation and validation
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if span enforcement compliant, error code on failure
 */
rift_result_t rift_governance_validate_span_enforcement(rift_governance_t *governance);

/**
 * Validate interference tolerance configuration
 * Ensures systematic interference management compliance
 * 
 * @param governance Governance state structure
 * @param measured_interference Current interference measurement
 * @return RIFT_SUCCESS if interference within tolerance, error code on failure
 */
rift_result_t rift_governance_validate_interference_tolerance(rift_governance_t *governance,
                                                              uint32_t measured_interference);

// =================================================================
// BUILD AND COMPILATION POLICY
// =================================================================

/**
 * Validate build policy compliance
 * Ensures systematic build methodology enforcement
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if build policy compliant, error code on failure
 */
rift_result_t rift_governance_validate_build_policy(rift_governance_t *governance);

/**
 * Validate artifact integrity requirements
 * Ensures systematic artifact validation and verification
 * 
 * @param governance Governance state structure
 * @param artifact_path Path to artifact for validation
 * @return RIFT_SUCCESS if artifact valid, error code on failure
 */
rift_result_t rift_governance_validate_artifact_integrity(rift_governance_t *governance,
                                                          const char *artifact_path);

/**
 * Validate test coverage requirements
 * Ensures systematic test coverage policy compliance
 * 
 * @param governance Governance state structure
 * @param measured_coverage Measured test coverage percentage
 * @return RIFT_SUCCESS if coverage sufficient, error code on failure
 */
rift_result_t rift_governance_validate_test_coverage(rift_governance_t *governance,
                                                     double measured_coverage);

/**
 * Validate documentation requirements
 * Ensures systematic documentation policy compliance
 * 
 * @param governance Governance state structure
 * @param documentation_path Path to documentation for validation
 * @return RIFT_SUCCESS if documentation compliant, error code on failure
 */
rift_result_t rift_governance_validate_documentation(rift_governance_t *governance,
                                                     const char *documentation_path);

// =================================================================
// COMPREHENSIVE VALIDATION INTERFACE
// =================================================================

/**
 * Execute complete AEGIS governance validation
 * Comprehensive validation of all systematic methodology requirements
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if full compliance achieved, error code on failure
 */
rift_result_t rift_governance_validate_complete(rift_governance_t *governance);

/**
 * Generate governance compliance report
 * Systematic report generation for audit and verification
 * 
 * @param governance Governance state structure
 * @param report_buffer Buffer for report generation
 * @param buffer_size Size of report buffer
 * @return RIFT_SUCCESS if report generated, error code on failure
 */
rift_result_t rift_governance_generate_compliance_report(rift_governance_t *governance,
                                                         char *report_buffer,
                                                         size_t buffer_size);

/**
 * Get governance validation summary
 * Returns systematic summary of validation results
 * 
 * @param governance Governance state structure
 * @param summary_buffer Buffer for summary generation
 * @param buffer_size Size of summary buffer
 * @return RIFT_SUCCESS if summary generated, error code on failure
 */
rift_result_t rift_governance_get_validation_summary(rift_governance_t *governance,
                                                     char *summary_buffer,
                                                     size_t buffer_size);

// =================================================================
// WATERFALL METHODOLOGY COMPLIANCE
// =================================================================

/**
 * Validate waterfall methodology compliance
 * Ensures systematic waterfall development process adherence
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS if waterfall compliant, error code on failure
 */
rift_result_t rift_governance_validate_waterfall_methodology(rift_governance_t *governance);

/**
 * Validate stage isolation requirements
 * Ensures systematic isolation between pipeline stages
 * 
 * @param governance Governance state structure
 * @param stage_id Pipeline stage identifier
 * @return RIFT_SUCCESS if isolation compliant, error code on failure
 */
rift_result_t rift_governance_validate_stage_isolation(rift_governance_t *governance,
                                                       uint32_t stage_id);

/**
 * Validate cross-stage communication
 * Ensures systematic communication protocol between stages
 * 
 * @param governance Governance state structure
 * @param source_stage Source stage identifier
 * @param target_stage Target stage identifier
 * @return RIFT_SUCCESS if communication compliant, error code on failure
 */
rift_result_t rift_governance_validate_cross_stage_communication(rift_governance_t *governance,
                                                                 uint32_t source_stage,
                                                                 uint32_t target_stage);

// =================================================================
// ERROR HANDLING AND DIAGNOSTICS
// =================================================================

/**
 * Get detailed error information for governance failure
 * Returns systematic error context and diagnostic information
 * 
 * @param governance Governance state structure
 * @param error_buffer Buffer for error message
 * @param buffer_size Size of error buffer
 * @return Length of error message written to buffer
 */
size_t rift_governance_get_error_details(rift_governance_t *governance,
                                         char *error_buffer,
                                         size_t buffer_size);

/**
 * Reset governance validation state
 * Systematic reset for re-validation procedures
 * 
 * @param governance Governance state structure
 * @return RIFT_SUCCESS on successful reset, error code on failure
 */
rift_result_t rift_governance_reset_validation_state(rift_governance_t *governance);

/**
 * Enable governance debug mode
 * Activates detailed diagnostic logging and validation tracing
 * 
 * @param governance Governance state structure
 * @param debug_level Debug verbosity level (0-3)
 * @return RIFT_SUCCESS on successful activation, error code on failure
 */
rift_result_t rift_governance_enable_debug_mode(rift_governance_t *governance,
                                                uint32_t debug_level);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_GOVERNANCE_H */

/*
 * =================================================================
 * AEGIS GOVERNANCE IMPLEMENTATION REQUIREMENTS
 * =================================================================
 * 
 * Implementation Strategy:
 * 
 * 1. **Configuration Management**: Implement robust parsing of .riftrc
 *    and pkg.riftrc configuration files with systematic error handling.
 * 
 * 2. **Memory Validation**: Implement comprehensive memory alignment
 *    validation with support for 4096-bit classical and 8-qubit quantum
 *    fallback configurations.
 * 
 * 3. **Token Schema Enforcement**: Implement systematic validation of
 *    token_type, token_value, token_memory triplet requirements with
 *    SHA-256 integrity verification.
 * 
 * 4. **Zero Trust Policy**: Implement comprehensive zero trust validation
 *    with span enforcement and interference tolerance management.
 * 
 * 5. **Waterfall Methodology**: Implement systematic validation of
 *    waterfall development process compliance including stage isolation
 *    and cross-stage communication protocols.
 * 
 * Required Implementation Files:
 * - src/governance/config.c         (Configuration management)
 * - src/governance/memory.c         (Memory validation)
 * - src/governance/tokens.c         (Token schema validation)
 * - src/governance/zero_trust.c     (Zero trust policy)
 * - src/governance/waterfall.c      (Waterfall methodology)
 * - src/governance/validation.c     (Comprehensive validation)
 * - src/governance/reporting.c      (Compliance reporting)
 * - src/governance/diagnostics.c    (Error handling and debugging)
 * 
 * =================================================================
 */

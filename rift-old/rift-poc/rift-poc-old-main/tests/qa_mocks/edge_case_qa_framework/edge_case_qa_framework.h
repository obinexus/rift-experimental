/**
 * @file edge_case_qa_framework.h
 * @brief RIFT Edge Case QA Validation Framework
 * 
 * Systematic boundary condition analysis for the RIFT pipeline
 * implementing comprehensive edge case detection and validation
 * within the AEGIS waterfall methodology.
 * 
 * @copyright Copyright (c) 2025 OBINexus Computing
 * @license Proprietary - All Rights Reserved
 */

#ifndef RIFT_EDGE_CASE_QA_FRAMEWORK_H
#define RIFT_EDGE_CASE_QA_FRAMEWORK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

// ============================================================================
// EDGE CASE CLASSIFICATION SYSTEM
// ============================================================================

typedef enum {
    EDGE_CASE_CATEGORY_MEMORY_GOVERNANCE,
    EDGE_CASE_CATEGORY_ENTROPY_VALIDATION,
    EDGE_CASE_CATEGORY_STAGE_TRANSITION,
    EDGE_CASE_CATEGORY_CRYPTOGRAPHIC_BOUNDARY,
    EDGE_CASE_CATEGORY_AUTOMATON_DEGENERATE,
    EDGE_CASE_CATEGORY_TELEMETRY_DISRUPTION,
    EDGE_CASE_CATEGORY_CONTEXT_VALIDATION,
    EDGE_CASE_CATEGORY_GOVERNANCE_CONTRACT,
    EDGE_CASE_CATEGORY_CONCURRENCY_BOUNDARY,
    EDGE_CASE_CATEGORY_ANTI_REVERSION_BYPASS
} edge_case_category_t;

typedef enum {
    EDGE_CASE_SEVERITY_LOW,          // System degradation, recoverable
    EDGE_CASE_SEVERITY_MEDIUM,       // Functional impact, workaround exists
    EDGE_CASE_SEVERITY_HIGH,         // Critical functionality affected
    EDGE_CASE_SEVERITY_CRITICAL,     // Security boundary compromise
    EDGE_CASE_SEVERITY_CATASTROPHIC  // System integrity failure
} edge_case_severity_t;

typedef enum {
    EDGE_CASE_STATUS_UNTESTED,
    EDGE_CASE_STATUS_PARTIALLY_TESTED,
    EDGE_CASE_STATUS_VALIDATED,
    EDGE_CASE_STATUS_MITIGATED,
    EDGE_CASE_STATUS_DOCUMENTED_RISK,
    EDGE_CASE_STATUS_REQUIRES_ARCHITECTURE_CHANGE
} edge_case_status_t;

// ============================================================================
// EDGE CASE DESCRIPTOR STRUCTURE
// ============================================================================

typedef struct edge_case_descriptor {
    uint64_t edge_case_id;
    edge_case_category_t category;
    edge_case_severity_t severity;
    edge_case_status_t status;
    
    char name[128];
    char description[512];
    char reproduction_steps[1024];
    char mitigation_strategy[512];
    char validation_procedure[512];
    
    // RIFT-specific metadata
    uint32_t affected_rift_stages;        // Bitmask of affected .rift stages
    bool affects_zero_trust_pipeline;
    bool affects_memory_governance;
    bool affects_ast_contextualization;
    bool affects_tennis_fsm_optimization;
    
    // Testing and validation metadata
    time_t discovered_date;
    time_t last_validated_date;
    char discovered_by[64];
    char validated_by[64];
    
    // Automation status
    bool automated_detection_available;
    bool automated_validation_available;
    char automation_script_path[256];
    
    // Risk assessment
    double probability_of_occurrence;    // 0.0 to 1.0
    double impact_magnitude;             // 0.0 to 1.0 
    double risk_score;                   // Calculated: probability * impact
    
} edge_case_descriptor_t;

// ============================================================================
// BOUNDARY CONDITION TESTING FRAMEWORK
// ============================================================================

typedef struct boundary_test_config {
    // Memory boundaries
    size_t min_memory_allocation;
    size_t max_memory_allocation;
    size_t memory_alignment_boundary;
    
    // Entropy boundaries
    double min_entropy_threshold;
    double max_entropy_threshold;
    double entropy_tolerance;
    
    // Stage transition boundaries
    uint32_t min_stage_level;
    uint32_t max_stage_level;
    uint32_t concurrent_transition_limit;
    
    // Timing boundaries
    uint64_t max_validation_time_ms;
    uint64_t telemetry_timeout_ms;
    uint64_t governance_decision_timeout_ms;
    
    // Concurrency boundaries
    uint32_t max_concurrent_tokens;
    uint32_t max_concurrent_validations;
    uint32_t thread_pool_limit;
    
} boundary_test_config_t;

// ============================================================================
// EDGE CASE VALIDATION ENGINE
// ============================================================================

typedef struct edge_case_validation_engine {
    edge_case_descriptor_t **edge_cases;
    size_t edge_case_count;
    size_t edge_case_capacity;
    
    boundary_test_config_t *boundary_config;
    
    // Validation statistics
    uint64_t total_validations_run;
    uint64_t validations_passed;
    uint64_t validations_failed;
    uint64_t critical_edge_cases_found;
    
    // Automation infrastructure
    bool automation_enabled;
    char automation_log_path[256];
    
    // Integration with RIFT pipeline
    bool rift_integration_enabled;
    void *telemetry_integration;
    void *governance_integration;
    
} edge_case_validation_engine_t;

// ============================================================================
// RIFT-SPECIFIC EDGE CASE DEFINITIONS
// ============================================================================

// Memory Governance Edge Cases
typedef struct memory_governance_edge_cases {
    // Token lifecycle edge cases
    bool test_token_creation_during_memory_exhaustion;
    bool test_token_destruction_during_governance_validation;
    bool test_concurrent_token_access_boundary;
    bool test_token_state_corruption_recovery;
    
    // Governance contract edge cases
    bool test_contract_circular_reference_detection;
    bool test_contract_missing_field_handling;
    bool test_contract_version_mismatch_resolution;
    bool test_contract_policy_contradiction_handling;
    
    // Anti-reversion edge cases
    bool test_governance_override_during_reversion_attempt;
    bool test_concurrent_reversion_requests;
    bool test_reversion_lock_timeout_handling;
    bool test_emergency_governance_mode_interaction;
    
} memory_governance_edge_cases_t;

// Entropy Validation Edge Cases
typedef struct entropy_validation_edge_cases {
    // Shannon entropy boundary conditions
    bool test_zero_entropy_handling;
    bool test_maximum_entropy_validation;
    bool test_entropy_calculation_overflow;
    bool test_entropy_threshold_exact_boundary;
    
    // Context-aware validation edge cases
    bool test_context_checksum_collision_handling;
    bool test_divisor_echo_alignment_overflow;
    bool test_perfect_number_validation_edge_cases;
    bool test_high_entropy_zone_detection_boundaries;
    
    // Entropy distribution edge cases
    bool test_entropy_spike_during_stage_transition;
    bool test_entropy_distribution_variance_limits;
    bool test_malformed_entropy_signature_handling;
    bool test_entropy_validation_timeout_scenarios;
    
} entropy_validation_edge_cases_t;

// Stage Transition Edge Cases
typedef struct stage_transition_edge_cases {
    // Progressive validation edge cases
    bool test_rapid_successive_transitions;
    bool test_transition_during_telemetry_failure;
    bool test_transition_during_cryptographic_operation;
    bool test_rollback_during_partial_transition;
    
    // Security scaling edge cases
    bool test_security_requirement_contradiction;
    bool test_validation_timeout_during_transition;
    bool test_concurrent_validation_conflicts;
    bool test_emergency_transition_procedures;
    
} stage_transition_edge_cases_t;

// ============================================================================
// AUTOMATED EDGE CASE DETECTION FUNCTIONS
// ============================================================================

/**
 * @brief Initialize edge case validation engine
 * @param config Boundary testing configuration
 * @return Initialized validation engine or NULL on failure
 */
edge_case_validation_engine_t* edge_case_engine_create(
    const boundary_test_config_t *config);

/**
 * @brief Register a new edge case for validation
 * @param engine Edge case validation engine
 * @param edge_case Edge case descriptor to register
 * @return true if registration successful, false otherwise
 */
bool edge_case_register(
    edge_case_validation_engine_t *engine,
    const edge_case_descriptor_t *edge_case);

/**
 * @brief Execute comprehensive edge case validation suite
 * @param engine Edge case validation engine
 * @param rift_stage Target RIFT stage for validation
 * @return Validation results summary
 */
typedef struct {
    uint64_t total_cases_tested;
    uint64_t cases_passed;
    uint64_t cases_failed;
    uint64_t critical_failures;
    edge_case_severity_t highest_severity_found;
    char summary_report[2048];
} edge_case_validation_results_t;

edge_case_validation_results_t edge_case_validate_all(
    edge_case_validation_engine_t *engine,
    uint32_t rift_stage);

/**
 * @brief Validate specific edge case category
 * @param engine Edge case validation engine  
 * @param category Edge case category to validate
 * @param rift_stage Target RIFT stage
 * @return Category-specific validation results
 */
edge_case_validation_results_t edge_case_validate_category(
    edge_case_validation_engine_t *engine,
    edge_case_category_t category,
    uint32_t rift_stage);

// ============================================================================
// MEMORY GOVERNANCE EDGE CASE VALIDATION
// ============================================================================

/**
 * @brief Test memory token creation under extreme conditions
 * @param token_size Size of memory token to create
 * @param available_memory Available system memory
 * @return true if edge case handled gracefully
 */
bool test_token_creation_memory_exhaustion(
    size_t token_size,
    size_t available_memory);

/**
 * @brief Test governance contract with missing critical fields
 * @param contract Governance contract with missing fields
 * @return true if missing fields detected and handled appropriately
 */
bool test_governance_contract_missing_fields(
    const void *incomplete_contract);

/**
 * @brief Test concurrent anti-reversion attempts
 * @param token Memory token to test
 * @param concurrent_attempts Number of simultaneous reversion attempts
 * @return true if concurrency handled correctly
 */
bool test_concurrent_anti_reversion_attempts(
    void *token,
    uint32_t concurrent_attempts);

// ============================================================================
// ENTROPY VALIDATION EDGE CASE TESTING
// ============================================================================

/**
 * @brief Test Shannon entropy calculation with zero-byte regions
 * @param memory_region Memory region containing zeros
 * @param region_size Size of memory region
 * @return true if zero entropy handled correctly
 */
bool test_zero_entropy_calculation(
    const void *memory_region,
    size_t region_size);

/**
 * @brief Test context checksum validation during entropy spikes
 * @param baseline_entropy Normal entropy level
 * @param spike_entropy Spike entropy level
 * @return true if entropy spike handled appropriately
 */
bool test_context_validation_entropy_spike(
    double baseline_entropy,
    double spike_entropy);

/**
 * @brief Test divisor echo alignment with large prime factors
 * @param test_number Number with large prime factors
 * @param stage_level RIFT stage level for testing
 * @return true if large prime handling is correct
 */
bool test_divisor_echo_large_primes(
    uint64_t test_number,
    uint32_t stage_level);

// ============================================================================
// STAGE TRANSITION EDGE CASE TESTING
// ============================================================================

/**
 * @brief Test rapid successive stage transitions
 * @param initial_stage Starting RIFT stage
 * @param target_stage Target RIFT stage
 * @param transition_interval_ms Interval between transitions
 * @return true if rapid transitions handled correctly
 */
bool test_rapid_stage_transitions(
    uint32_t initial_stage,
    uint32_t target_stage,
    uint64_t transition_interval_ms);

/**
 * @brief Test stage transition during telemetry failure
 * @param stage_transition Stage transition to attempt
 * @param telemetry_failure_type Type of telemetry failure to simulate
 * @return true if transition handles telemetry failure gracefully
 */
bool test_transition_telemetry_failure(
    uint32_t stage_transition,
    uint32_t telemetry_failure_type);

// ============================================================================
// AUTOMATED BOUNDARY DETECTION
// ============================================================================

/**
 * @brief Automatically detect system boundaries for edge case testing
 * @param system_config Current system configuration
 * @return Detected boundary configuration for testing
 */
boundary_test_config_t* detect_system_boundaries(
    const void *system_config);

/**
 * @brief Generate comprehensive edge case test suite
 * @param engine Edge case validation engine
 * @param boundary_config Boundary testing configuration
 * @return true if test suite generation successful
 */
bool generate_comprehensive_edge_case_suite(
    edge_case_validation_engine_t *engine,
    const boundary_test_config_t *boundary_config);

/**
 * @brief Execute continuous edge case monitoring
 * @param engine Edge case validation engine
 * @param monitoring_interval_ms Monitoring interval in milliseconds
 * @return true if monitoring started successfully
 */
bool start_continuous_edge_case_monitoring(
    edge_case_validation_engine_t *engine,
    uint64_t monitoring_interval_ms);

// ============================================================================
// INTEGRATION WITH RIFT PIPELINE
// ============================================================================

/**
 * @brief Integrate edge case validation with RIFT telemetry system
 * @param engine Edge case validation engine
 * @param telemetry_system RIFT telemetry integration
 * @return true if integration successful
 */
bool integrate_edge_case_telemetry(
    edge_case_validation_engine_t *engine,
    void *telemetry_system);

/**
 * @brief Integrate edge case validation with governance system
 * @param engine Edge case validation engine
 * @param governance_system Memory governance integration
 * @return true if integration successful  
 */
bool integrate_edge_case_governance(
    edge_case_validation_engine_t *engine,
    void *governance_system);

/**
 * @brief Generate edge case validation report for QA certification
 * @param engine Edge case validation engine
 * @param rift_stage Target RIFT stage
 * @param report_buffer Buffer for report output
 * @param buffer_size Size of report buffer
 * @return Number of bytes written to report buffer
 */
size_t generate_edge_case_qa_report(
    const edge_case_validation_engine_t *engine,
    uint32_t rift_stage,
    char *report_buffer,
    size_t buffer_size);

// ============================================================================
// RESOURCE MANAGEMENT
// ============================================================================

/**
 * @brief Free edge case validation engine resources
 * @param engine Edge case validation engine to free
 */
void edge_case_engine_free(edge_case_validation_engine_t *engine);

/**
 * @brief Free edge case descriptor resources
 * @param edge_case Edge case descriptor to free
 */
void edge_case_descriptor_free(edge_case_descriptor_t *edge_case);

/**
 * @brief Free boundary test configuration resources
 * @param config Boundary test configuration to free
 */
void boundary_test_config_free(boundary_test_config_t *config);

// ============================================================================
// EDGE CASE MACROS FOR VALIDATION
// ============================================================================

/**
 * @brief Macro for edge case assertion with telemetry logging
 */
#define EDGE_CASE_ASSERT(condition, edge_case_id, message) \
    do { \
        if (!(condition)) { \
            log_edge_case_failure(edge_case_id, message, __FILE__, __LINE__); \
            return false; \
        } \
    } while(0)

/**
 * @brief Macro for boundary condition testing
 */
#define BOUNDARY_TEST(value, min_bound, max_bound, edge_case_id) \
    do { \
        if ((value) < (min_bound) || (value) > (max_bound)) { \
            log_boundary_violation(edge_case_id, value, min_bound, max_bound); \
            return false; \
        } \
    } while(0)

/**
 * @brief Macro for timing-sensitive edge case validation
 */
#define EDGE_CASE_TIMEOUT_TEST(operation, timeout_ms, edge_case_id) \
    do { \
        uint64_t start_time = get_current_time_ms(); \
        operation; \
        uint64_t elapsed = get_current_time_ms() - start_time; \
        if (elapsed > timeout_ms) { \
            log_timeout_violation(edge_case_id, elapsed, timeout_ms); \
            return false; \
        } \
    } while(0)

#endif /* RIFT_EDGE_CASE_QA_FRAMEWORK_H */

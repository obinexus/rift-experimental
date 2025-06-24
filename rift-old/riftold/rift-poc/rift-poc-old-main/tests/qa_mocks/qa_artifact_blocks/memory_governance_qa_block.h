#ifndef QA_MEMORY_GOVERNANCE_BLOCK_H
#define QA_MEMORY_GOVERNANCE_BLOCK_H

/**
 * @file memory_governance_qa_block.h
 * @brief QA_ARTIFACT_BLOCK - Memory Governance Component
 * 
 * RIFT Pipeline Quality Assurance Framework
 * OBINexus Computing - AEGIS Waterfall Methodology
 * 
 * @copyright Copyright (c) 2025 OBINexus Computing
 * @license Proprietary - All Rights Reserved
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
 * ============================================================================
 * QA_ARTIFACT_BLOCK - COMPREHENSIVE QQ COMPLIANCE
 * ============================================================================
 * 
 * GOVERNANCE NOTICE: This artifact is governed by the Comprehensive QQ 
 * (Quality Qualification) standard defined for the RIFT-POC project pipeline.
 * 
 * REQUIREMENT: Integration testing is MANDATORY for all RIFT artifacts.
 * End-to-End testing is NOT REQUIRED for RIFT pipeline components.
 */

// ============================================================================
// ARTIFACT IDENTIFICATION
// ============================================================================
#define QA_ARTIFACT_NAME "Memory Governance Token System"
#define QA_ARTIFACT_TYPE "MEMORY_GOVERNANCE"
#define QA_RIFT_STAGE_TARGET ".rift.4"
#define QA_COMPONENT_CATEGORY "CORE"

// ============================================================================
// TEST PROCEDURE CONFIGURATION
// ============================================================================
#define QA_TEST_PROCEDURE_REFERENCE "test_procedures/memory_governance_test_procedure.md"
#define QA_TEST_FRAMEWORK "custom"
#define QA_MINIMUM_QA_LEVEL "INTEGRATION_TESTING_REQUIRED"
#define QA_E2E_TESTING_REQUIRED false

// ============================================================================
// TESTING COVERAGE MATRIX
// ============================================================================
typedef struct qa_manual_testing {
    bool enabled;
    const char* procedure_file;
    const char* last_execution;
    const char* status;
    const char* reviewer;
} qa_manual_testing_t;

typedef struct qa_unit_testing {
    bool enabled;
    const char* test_files;
    int coverage_target;
    int actual_coverage;
    const char* status;
    const char* framework;
} qa_unit_testing_t;

typedef struct qa_integration_testing {
    bool enabled; // MANDATORY: true
    const char* test_files;
    const char* pipeline_stage_tests;
    const char* context_validation_tests;
    const char* memory_governance_tests;
    const char* status;
    const char* last_execution;
} qa_integration_testing_t;

// ============================================================================
// MEMORY GOVERNANCE SPECIFIC VALIDATION
// ============================================================================
typedef struct qa_memory_governance_validation {
    const char* memory_token_validation;
    const char* governance_contract_test;
    const char* violation_logging_test;
    const char* anti_reversion_test;
    const char* stage_transition_test;
} qa_memory_governance_validation_t;

// ============================================================================
// QA REVIEW AND APPROVAL
// ============================================================================
typedef struct qa_review_status {
    const char* status; // DRAFT|READY|AUDITED|APPROVED|REJECTED
    const char* review_date;
    const char* reviewer;
    const char* approval_authority;
} qa_review_status_t;

// ============================================================================
// QA VALIDATION FUNCTION PROTOTYPES
// ============================================================================
bool qa_validate_memory_governance_compliance(void);
bool qa_execute_integration_tests(void);
bool qa_validate_stage_progression(int target_stage);
void qa_log_validation_outcome(const char* test_name, bool result);

#endif /* QA_MEMORY_GOVERNANCE_BLOCK_H */

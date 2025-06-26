/**
 * @file test_integration_memory_governance.c
 * @brief Integration test suite for Memory Governance Token System
 * 
 * This file demonstrates proper integration testing for RIFT Memory Governance
 * components following the Comprehensive QQ framework and AEGIS methodology.
 * 
 * @copyright Copyright (c) 2025 OBINexus Computing
 * @license Proprietary - All Rights Reserved
 */

#include "memory_governance.h"
#include "telemetry_governance.h"
#include "../qa_artifact_blocks/memory_governance_qa_block.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

/*
 * QA_ARTIFACT_BLOCK INTEGRATION
 * This test file is governed by the Memory Governance QA Block
 * defined in qa_artifact_blocks/memory_governance_qa_block.h
 * 
 * COMPLIANCE: Comprehensive QQ Framework
 * METHODOLOGY: AEGIS Waterfall with systematic quality gates
 */

// ============================================================================
// TEST EXECUTION FRAMEWORK
// ============================================================================

static int tests_passed = 0;
static int tests_failed = 0;
static int total_tests = 0;

typedef enum {
    QA_TEST_RESULT_PASS,
    QA_TEST_RESULT_FAIL,
    QA_TEST_RESULT_SKIP
} qa_test_result_t;

/*
 * QA Telemetry Integration - Systematic Validation Logging
 */
void qa_log_test_result(const char* test_name, qa_test_result_t result) {
    total_tests++;
    
    switch (result) {
        case QA_TEST_RESULT_PASS:
            tests_passed++;
            log_governance_event(TELEMETRY_LEVEL_INFO, 0, 
                               "QA_INTEGRATION_TEST_PASS: %s", test_name);
            printf("[QA-PASS] %s\n", test_name);
            break;
            
        case QA_TEST_RESULT_FAIL:
            tests_failed++;
            log_governance_event(TELEMETRY_LEVEL_ERROR, 0, 
                               "QA_INTEGRATION_TEST_FAIL: %s", test_name);
            printf("[QA-FAIL] %s\n", test_name);
            break;
            
        case QA_TEST_RESULT_SKIP:
            log_governance_event(TELEMETRY_LEVEL_WARN, 0, 
                               "QA_INTEGRATION_TEST_SKIP: %s", test_name);
            printf("[QA-SKIP] %s\n", test_name);
            break;
    }
}

// ============================================================================
// INTEGRATION TEST IMPLEMENTATIONS
// ============================================================================

/*
 * Integration Test 1: Memory Token Lifecycle Validation
 * MANDATORY: Tests complete token lifecycle with entropy validation
 * AEGIS COMPLIANCE: Data-oriented token management verification
 */
qa_test_result_t test_memory_token_lifecycle_validation(void) {
    printf("\n=== Integration Test: Memory Token Lifecycle ===\n");
    
    // Test data preparation with controlled entropy
    size_t memory_size = 1024;
    void* memory_region = malloc(memory_size);
    if (!memory_region) {
        printf("ERROR: Failed to allocate test memory\n");
        return QA_TEST_RESULT_FAIL;
    }
    
    // Create high-entropy test pattern for context-aware validation
    for (size_t i = 0; i < memory_size; i++) {
        ((uint8_t*)memory_region)[i] = (uint8_t)(rand() % 256);
    }
    
    // Stage 4 token creation (hardened + encrypted + context-validated)
    rift_memory_token_t* token = rift_memory_token_create(4, memory_size, memory_region);
    if (!token) {
        printf("ERROR: Failed to create memory token\n");
        free(memory_region);
        return QA_TEST_RESULT_FAIL;
    }
    
    // Systematic validation of token properties
    bool lifecycle_valid = true;
    
    // Core property validation
    lifecycle_valid &= (token->stage_level == 4);
    lifecycle_valid &= (token->allocated_bytes == memory_size);
    lifecycle_valid &= (token->memory_region == memory_region);
    lifecycle_valid &= (token->token_id > 0);
    
    // Cryptographic signature validation
    lifecycle_valid &= (token->entropy_signature > 0);
    lifecycle_valid &= (token->context_checksum > 0);
    
    // Stage 4 specific requirements
    lifecycle_valid &= (token->governance_flags != 0);
    lifecycle_valid &= (strlen(token->stage_signature) > 0);
    
    printf("Token Validation Results:\n");
    printf("  Token ID: %lu\n", token->token_id);
    printf("  Stage Level: %u (target: 4)\n", token->stage_level);
    printf("  Memory Size: %zu bytes\n", token->allocated_bytes);
    printf("  Entropy Signature: %lu\n", token->entropy_signature);
    printf("  Context Checksum: 0x%016lx\n", token->context_checksum);
    printf("  Governance Flags: 0x%08x\n", token->governance_flags);
    
    // Resource cleanup
    rift_memory_token_free(token);
    free(memory_region);
    
    return lifecycle_valid ? QA_TEST_RESULT_PASS : QA_TEST_RESULT_FAIL;
}

/*
 * Integration Test 2: Governance Contract Enforcement
 * MANDATORY: Tests systematic contract validation across RIFT stages
 * WATERFALL COMPLIANCE: Design validation through structured testing
 */
qa_test_result_t test_governance_contract_enforcement(void) {
    printf("\n=== Integration Test: Governance Contract Enforcement ===\n");
    
    // Create comprehensive governance contract for testing
    rift_memory_governance_contract_t contract = {
        .contract_version = 1,
        .minimum_security_level = 3,
        .governance_policy_hash = 0xDEADBEEFCAFEBABE,
        .max_memory_allocation = 1024 * 1024, // 1MB limit
        .allowed_stage_transitions = RIFT_TRANSITION_ALLOW_3_TO_4 | RIFT_TRANSITION_ALLOW_4_TO_5,
        .enforce_anti_reversion = true,
        .entropy_threshold_percent = 75
    };
    strcpy(contract.policy_name, "QA Integration Test Policy");
    
    // Initialize governance engine with contract
    rift_memory_governance_engine_t* engine = rift_memory_governance_engine_create(&contract);
    if (!engine) {
        printf("ERROR: Failed to create governance engine\n");
        return QA_TEST_RESULT_FAIL;
    }
    
    // Systematic contract validation
    bool contract_enforcement_valid = true;
    
    // Engine initialization validation
    contract_enforcement_valid &= (engine->active_contract != NULL);
    contract_enforcement_valid &= (engine->governance_active == true);
    contract_enforcement_valid &= (engine->token_registry != NULL);
    contract_enforcement_valid &= (engine->violation_log != NULL);
    
    // Contract property validation
    contract_enforcement_valid &= (engine->active_contract->minimum_security_level == 3);
    contract_enforcement_valid &= (engine->active_contract->enforce_anti_reversion == true);
    contract_enforcement_valid &= (engine->active_contract->entropy_threshold_percent == 75);
    
    // Policy enforcement validation
    contract_enforcement_valid &= (strcmp(engine->active_contract->policy_name, "QA Integration Test Policy") == 0);
    
    printf("Contract Enforcement Results:\n");
    printf("  Contract Version: %u\n", engine->active_contract->contract_version);
    printf("  Minimum Security Level: %u\n", engine->active_contract->minimum_security_level);
    printf("  Anti-Reversion Enforced: %s\n", 
           engine->active_contract->enforce_anti_reversion ? "YES" : "NO");
    printf("  Entropy Threshold: %u%%\n", engine->active_contract->entropy_threshold_percent);
    printf("  Policy Name: %s\n", engine->active_contract->policy_name);
    printf("  Engine State: %s\n", engine->governance_active ? "ACTIVE" : "INACTIVE");
    
    // Resource cleanup
    rift_memory_governance_engine_free(engine);
    
    return contract_enforcement_valid ? QA_TEST_RESULT_PASS : QA_TEST_RESULT_FAIL;
}

/*
 * Integration Test 3: Progressive Stage Validation
 * MANDATORY: Tests systematic quality progression across RIFT stages
 * PROGRESSIVE QA: Validates increasing validation requirements
 */
qa_test_result_t test_progressive_stage_validation(void) {
    printf("\n=== Integration Test: Progressive Stage Validation ===\n");
    
    // Test memory allocation
    size_t memory_size = 512;
    void* memory_region = malloc(memory_size);
    if (!memory_region) {
        printf("ERROR: Failed to allocate test memory\n");
        return QA_TEST_RESULT_FAIL;
    }
    
    // Create controlled entropy pattern
    memset(memory_region, 0xCC, memory_size);
    
    bool progressive_validation_passed = true;
    const uint32_t test_stages[] = {0, 1, 3, 4, 5};
    const size_t num_stages = sizeof(test_stages) / sizeof(test_stages[0]);
    
    for (size_t i = 0; i < num_stages; i++) {
        uint32_t stage = test_stages[i];
        
        printf("  Testing Stage %u validation...\n", stage);
        
        // Create token for current stage
        rift_memory_token_t* token = rift_memory_token_create(stage, memory_size, memory_region);
        if (!token) {
            printf("    ERROR: Failed to create token for stage %u\n", stage);
            progressive_validation_passed = false;
            continue;
        }
        
        // Create stage-appropriate governance contract
        rift_memory_governance_contract_t contract = {
            .contract_version = 1,
            .minimum_security_level = stage,
            .max_memory_allocation = 1024 * 1024,
            .allowed_stage_transitions = 0xFF, // Allow all for testing
            .enforce_anti_reversion = (stage >= 4),
            .entropy_threshold_percent = 50 + (stage * 8) // Progressive requirements
        };
        
        // Stage-specific validation
        bool stage_validation_passed = false;
        switch (stage) {
            case 0:
                stage_validation_passed = verify_stage_0_memory_governance(token, &contract);
                break;
            case 1:
                stage_validation_passed = verify_stage_1_memory_governance(token, &contract);
                break;
            case 3:
                stage_validation_passed = verify_stage_3_memory_governance(token, &contract);
                break;
            case 4:
                stage_validation_passed = verify_stage_4_memory_governance(token, &contract);
                break;
            case 5:
                stage_validation_passed = verify_stage_5_memory_governance(token, &contract);
                break;
        }
        
        printf("    Stage %u: %s (entropy threshold: %u%%)\n", 
               stage, stage_validation_passed ? "PASS" : "FAIL", 
               contract.entropy_threshold_percent);
        
        progressive_validation_passed &= stage_validation_passed;
        
        // Cleanup
        rift_memory_token_free(token);
    }
    
    free(memory_region);
    return progressive_validation_passed ? QA_TEST_RESULT_PASS : QA_TEST_RESULT_FAIL;
}

/*
 * Integration Test 4: Anti-Reversion Protection
 * MANDATORY: Tests forward-only stage progression enforcement
 * SECURITY MODEL: Validates transversal stage mapping protection
 */
qa_test_result_t test_anti_reversion_protection(void) {
    printf("\n=== Integration Test: Anti-Reversion Protection ===\n");
    
    // Create test memory for Stage 5 token
    void* memory_region = malloc(256);
    if (!memory_region) {
        printf("ERROR: Failed to allocate test memory\n");
        return QA_TEST_RESULT_FAIL;
    }
    
    memset(memory_region, 0xFF, 256); // High entropy pattern
    
    // Create Stage 5 token (highest security level)
    rift_memory_token_t* token = rift_memory_token_create(5, 256, memory_region);
    if (!token) {
        printf("ERROR: Failed to create Stage 5 token\n");
        free(memory_region);
        return QA_TEST_RESULT_FAIL;
    }
    
    // Enable anti-reversion lock
    token->anti_reversion_lock = true;
    
    bool anti_reversion_validation_passed = true;
    
    // Test backward transitions (should be blocked)
    const uint32_t forbidden_stages[] = {4, 3, 1, 0};
    const size_t num_forbidden = sizeof(forbidden_stages) / sizeof(forbidden_stages[0]);
    
    for (size_t i = 0; i < num_forbidden; i++) {
        uint32_t target_stage = forbidden_stages[i];
        
        // Test without governance override (should fail)
        bool reversion_blocked = !check_memory_governance_reversion(token, target_stage, false);
        anti_reversion_validation_passed &= reversion_blocked;
        
        printf("  Stage 5→%u reversion blocked: %s\n", 
               target_stage, reversion_blocked ? "PASS" : "FAIL");
        
        // Test with governance override (should succeed)
        bool governance_override_works = check_memory_governance_reversion(token, target_stage, true);
        anti_reversion_validation_passed &= governance_override_works;
        
        printf("  Stage 5→%u governance override: %s\n", 
               target_stage, governance_override_works ? "PASS" : "FAIL");
    }
    
    // Cleanup
    rift_memory_token_free(token);
    free(memory_region);
    
    return anti_reversion_validation_passed ? QA_TEST_RESULT_PASS : QA_TEST_RESULT_FAIL;
}

// ============================================================================
// QA FRAMEWORK INTEGRATION TEST EXECUTION
// ============================================================================

/*
 * Main QA Integration Test Suite Execution
 * COMPREHENSIVE QQ COMPLIANCE: Systematic validation following AEGIS methodology
 */
int main(void) {
    printf("=============================================================\n");
    printf("RIFT Memory Governance Integration Test Suite\n");
    printf("Comprehensive Quality Qualification Framework Validation\n");
    printf("AEGIS Waterfall Methodology Compliance Testing\n");
    printf("=============================================================\n");
    
    // Initialize random seed for entropy testing
    srand((unsigned int)time(NULL));
    
    // Initialize telemetry for QA validation logging
    log_governance_event(TELEMETRY_LEVEL_INFO, 0, 
                        "QA_INTEGRATION_TEST_SUITE_STARTED: Memory Governance Framework");
    
    // Execute mandatory integration tests in systematic order
    qa_log_test_result("Memory Token Lifecycle Validation", 
                       test_memory_token_lifecycle_validation());
    
    qa_log_test_result("Governance Contract Enforcement", 
                       test_governance_contract_enforcement());
    
    qa_log_test_result("Progressive Stage Validation", 
                       test_progressive_stage_validation());
    
    qa_log_test_result("Anti-Reversion Protection", 
                       test_anti_reversion_protection());
    
    // Generate comprehensive QA validation summary
    printf("\n=============================================================\n");
    printf("QA Integration Test Suite Results:\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Total Tests: %d\n", total_tests);
    printf("Success Rate: %.1f%%\n", 
           total_tests > 0 ? (float)tests_passed / total_tests * 100 : 0.0f);
    printf("=============================================================\n");
    
    // Determine overall QA certification status
    bool qa_certification_passed = (tests_failed == 0 && total_tests > 0);
    
    // Log final QA validation outcome to telemetry
    log_governance_event(qa_certification_passed ? TELEMETRY_LEVEL_INFO : TELEMETRY_LEVEL_ERROR, 0,
                        "QA_INTEGRATION_TEST_SUITE_COMPLETED: %s (%d/%d passed)",
                        qa_certification_passed ? "CERTIFIED" : "FAILED", 
                        tests_passed, total_tests);
    
    // Update QA artifact block certification status
    if (qa_certification_passed) {
        printf("\n✅ QA_ARTIFACT_BLOCK Status: INTEGRATION_TESTING_CERTIFIED\n");
        printf("✅ Component ready for RIFT stage progression\n");
        printf("✅ AEGIS waterfall methodology compliance verified\n");
    } else {
        printf("\n❌ QA_ARTIFACT_BLOCK Status: INTEGRATION_TESTING_FAILED\n");
        printf("❌ Component requires remediation before stage progression\n");
        printf("❌ Review failed tests and address issues before re-certification\n");
    }
    
    return qa_certification_passed ? 0 : 1;
}

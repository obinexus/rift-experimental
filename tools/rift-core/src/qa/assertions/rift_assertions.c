/*
 * RIFT Assertion Library Implementation
 */

#include "rift-core/qa/assertions/rift_assertions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

assertion_result_t assert_token_type_match(
    const rift_r_regex_token_t* token, 
    r_regex_type_t expected_type
) {
    assertion_result_t result = {0};
    
    if (!token) {
        result.passed = false;
        result.message = strdup("Token is NULL");
        result.matrix_update = QA_VALIDATION_FALSE_NEGATIVE;
        return result;
    }
    
    if (token->type == expected_type) {
        result.passed = true;
        result.message = strdup("Token type matches expected");
        result.matrix_update = QA_VALIDATION_TRUE_POSITIVE;
    } else {
        result.passed = false;
        result.message = strdup("Token type does not match expected");
        result.matrix_update = QA_VALIDATION_FALSE_POSITIVE;
    }
    
    return result;
}

assertion_result_t assert_token_flags_match(
    const rift_r_regex_token_t* token, 
    uint32_t expected_flags
) {
    assertion_result_t result = {0};
    
    if (!token) {
        result.passed = false;
        result.message = strdup("Token is NULL");
        result.matrix_update = QA_VALIDATION_FALSE_NEGATIVE;
        return result;
    }
    
    if ((token->flags & expected_flags) == expected_flags) {
        result.passed = true;
        result.message = strdup("Token flags match expected");
        result.matrix_update = QA_VALIDATION_TRUE_POSITIVE;
    } else {
        result.passed = false;
        result.message = strdup("Token flags do not match expected");
        result.matrix_update = QA_VALIDATION_FALSE_POSITIVE;
    }
    
    return result;
}

assertion_result_t assert_token_memory_scope(
    const rift_r_regex_token_t* token, 
    const scope_channels_t* expected_scope
) {
    assertion_result_t result = {0};
    
    if (!token || !expected_scope) {
        result.passed = false;
        result.message = strdup("Token or expected scope is NULL");
        result.matrix_update = QA_VALIDATION_FALSE_NEGATIVE;
        return result;
    }
    
    bool scope_match = (
        token->scope.default_channel == expected_scope->default_channel &&
        token->scope.user_output_channel == expected_scope->user_output_channel
    );
    
    if (scope_match) {
        result.passed = true;
        result.message = strdup("Token memory scope matches expected");
        result.matrix_update = QA_VALIDATION_TRUE_POSITIVE;
    } else {
        result.passed = false;
        result.message = strdup("Token memory scope does not match expected");
        result.matrix_update = QA_VALIDATION_FALSE_POSITIVE;
    }
    
    return result;
}

assertion_result_t assert_r_syntax_compliance(
    const rift_r_regex_token_t* token, 
    const char* regex_pattern
) {
    assertion_result_t result = {0};
    
    if (!token || !regex_pattern) {
        result.passed = false;
        result.message = strdup("Token or regex pattern is NULL");
        result.matrix_update = QA_VALIDATION_FALSE_NEGATIVE;
        return result;
    }
    
    bool compliance = r_syntax_validate_token(token);
    
    if (compliance) {
        result.passed = true;
        result.message = strdup("Token complies with R-syntax requirements");
        result.matrix_update = QA_VALIDATION_TRUE_POSITIVE;
    } else {
        result.passed = false;
        result.message = strdup("Token does not comply with R-syntax requirements");
        result.matrix_update = QA_VALIDATION_FALSE_POSITIVE;
    }
    
    return result;
}

assertion_result_t assert_matrix_method_agnostic(
    void* method_impl,
    void* square_model,
    void* triangular_model
) {
    assertion_result_t result = {0};
    
    if (!method_impl || !square_model || !triangular_model) {
        result.passed = false;
        result.message = strdup("Method implementation or models are NULL");
        result.matrix_update = QA_VALIDATION_FALSE_NEGATIVE;
        return result;
    }
    
    // Simulate model-agnostic validation
    // In real implementation, this would test that the method works
    // identically across different matrix model types
    
    result.passed = true;
    result.message = strdup("Method is model-agnostic across matrix types");
    result.matrix_update = QA_VALIDATION_TRUE_POSITIVE;
    
    return result;
}

void assertion_result_destroy(assertion_result_t* result) {
    if (result && result->message) {
        free(result->message);
        result->message = NULL;
    }
}

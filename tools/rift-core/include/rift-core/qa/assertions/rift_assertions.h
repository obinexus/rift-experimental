/*
 * RIFT Assertion Library for Type-Case Matching and Method Validation
 */

#ifndef RIFT_ASSERTIONS_H
#define RIFT_ASSERTIONS_H

#include "rift-core/qa/matrix/qa_workflow_matrix.h"
#include "rift-core/qa/r_syntax_tokenizer.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Assertion result
typedef struct {
    bool passed;
    char* message;
    qa_validation_result_t matrix_update;
} assertion_result_t;

// Type-case matching assertions
assertion_result_t assert_token_type_match(
    const rift_r_regex_token_t* token, 
    r_regex_type_t expected_type
);

assertion_result_t assert_token_flags_match(
    const rift_r_regex_token_t* token, 
    uint32_t expected_flags
);

assertion_result_t assert_token_memory_scope(
    const rift_r_regex_token_t* token, 
    const scope_channels_t* expected_scope
);

assertion_result_t assert_r_syntax_compliance(
    const rift_r_regex_token_t* token, 
    const char* regex_pattern
);

// Model-agnostic method assertions
assertion_result_t assert_matrix_method_agnostic(
    void* method_impl,
    void* square_model,
    void* triangular_model
);

// Assertion cleanup
void assertion_result_destroy(assertion_result_t* result);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_ASSERTIONS_H */

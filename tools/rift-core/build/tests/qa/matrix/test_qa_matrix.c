#include "rift-core/qa/matrix/qa_workflow_matrix.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    printf("QA Workflow Matrix Unit Tests\n");
    
    qa_workflow_matrix_t* matrix = qa_matrix_create();
    assert(matrix != NULL);
    
    // Test matrix updates
    qa_matrix_update(matrix, QA_VALIDATION_TRUE_POSITIVE);
    qa_matrix_update(matrix, QA_VALIDATION_TRUE_NEGATIVE);
    qa_matrix_update(matrix, QA_VALIDATION_FALSE_POSITIVE);
    qa_matrix_update(matrix, QA_VALIDATION_FALSE_NEGATIVE);
    
    assert(matrix->true_positive == 1);
    assert(matrix->true_negative == 1);
    assert(matrix->false_positive == 1);
    assert(matrix->false_negative == 1);
    
    // Test metrics calculation
    assert(matrix->accuracy == 0.5); // (TP + TN) / Total = (1 + 1) / 4 = 0.5
    
    qa_matrix_destroy(matrix);
    
    printf("✓ All QA matrix tests passed\n");
    return 0;
}

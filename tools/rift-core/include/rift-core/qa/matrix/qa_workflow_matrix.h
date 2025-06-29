/*
 * RIFT QA Workflow Matrix - Confusion Matrix Implementation
 * For comprehensive tokenization and stage validation
 */

#ifndef RIFT_QA_WORKFLOW_MATRIX_H
#define RIFT_QA_WORKFLOW_MATRIX_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// QA Workflow Matrix Structure
typedef struct {
    uint32_t true_positive;     // Correctly identified valid tokens
    uint32_t true_negative;     // Correctly rejected invalid tokens  
    uint32_t false_positive;    // Incorrectly accepted invalid tokens
    uint32_t false_negative;    // Incorrectly rejected valid tokens
    
    // Derived metrics
    double precision;           // TP / (TP + FP)
    double recall;              // TP / (TP + FN)
    double f1_score;            // 2 * (precision * recall) / (precision + recall)
    double accuracy;            // (TP + TN) / (TP + TN + FP + FN)
} qa_workflow_matrix_t;

// Validation result enumeration
typedef enum {
    QA_VALIDATION_TRUE_POSITIVE,
    QA_VALIDATION_TRUE_NEGATIVE,
    QA_VALIDATION_FALSE_POSITIVE,
    QA_VALIDATION_FALSE_NEGATIVE
} qa_validation_result_t;

// Matrix operations
qa_workflow_matrix_t* qa_matrix_create(void);
void qa_matrix_destroy(qa_workflow_matrix_t* matrix);
void qa_matrix_update(qa_workflow_matrix_t* matrix, qa_validation_result_t result);
void qa_matrix_calculate_metrics(qa_workflow_matrix_t* matrix);
void qa_matrix_reset(qa_workflow_matrix_t* matrix);
void qa_matrix_print_report(const qa_workflow_matrix_t* matrix);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_QA_WORKFLOW_MATRIX_H */

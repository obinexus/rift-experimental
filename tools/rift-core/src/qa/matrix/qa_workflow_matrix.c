/*
 * RIFT QA Workflow Matrix Implementation
 */

#include "rift-core/qa/matrix/qa_workflow_matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

qa_workflow_matrix_t* qa_matrix_create(void) {
    qa_workflow_matrix_t* matrix = calloc(1, sizeof(qa_workflow_matrix_t));
    if (!matrix) {
        return NULL;
    }
    
    qa_matrix_reset(matrix);
    return matrix;
}

void qa_matrix_destroy(qa_workflow_matrix_t* matrix) {
    if (matrix) {
        free(matrix);
    }
}

void qa_matrix_update(qa_workflow_matrix_t* matrix, qa_validation_result_t result) {
    if (!matrix) return;
    
    switch (result) {
        case QA_VALIDATION_TRUE_POSITIVE:
            matrix->true_positive++;
            break;
        case QA_VALIDATION_TRUE_NEGATIVE:
            matrix->true_negative++;
            break;
        case QA_VALIDATION_FALSE_POSITIVE:
            matrix->false_positive++;
            break;
        case QA_VALIDATION_FALSE_NEGATIVE:
            matrix->false_negative++;
            break;
    }
    
    qa_matrix_calculate_metrics(matrix);
}

void qa_matrix_calculate_metrics(qa_workflow_matrix_t* matrix) {
    if (!matrix) return;
    
    uint32_t tp = matrix->true_positive;
    uint32_t tn = matrix->true_negative;
    uint32_t fp = matrix->false_positive;
    uint32_t fn = matrix->false_negative;
    
    // Calculate precision: TP / (TP + FP)
    matrix->precision = (tp + fp > 0) ? (double)tp / (tp + fp) : 0.0;
    
    // Calculate recall: TP / (TP + FN)
    matrix->recall = (tp + fn > 0) ? (double)tp / (tp + fn) : 0.0;
    
    // Calculate F1-score: 2 * (precision * recall) / (precision + recall)
    matrix->f1_score = (matrix->precision + matrix->recall > 0) ? 
        2 * (matrix->precision * matrix->recall) / (matrix->precision + matrix->recall) : 0.0;
    
    // Calculate accuracy: (TP + TN) / (TP + TN + FP + FN)
    uint32_t total = tp + tn + fp + fn;
    matrix->accuracy = (total > 0) ? (double)(tp + tn) / total : 0.0;
}

void qa_matrix_reset(qa_workflow_matrix_t* matrix) {
    if (!matrix) return;
    
    memset(matrix, 0, sizeof(qa_workflow_matrix_t));
}

void qa_matrix_print_report(const qa_workflow_matrix_t* matrix) {
    if (!matrix) return;
    
    printf("\n=== QA Workflow Matrix Report ===\n");
    printf("True Positives:  %u\n", matrix->true_positive);
    printf("True Negatives:  %u\n", matrix->true_negative);
    printf("False Positives: %u\n", matrix->false_positive);
    printf("False Negatives: %u\n", matrix->false_negative);
    printf("\n--- Derived Metrics ---\n");
    printf("Precision: %.4f\n", matrix->precision);
    printf("Recall:    %.4f\n", matrix->recall);
    printf("F1-Score:  %.4f\n", matrix->f1_score);
    printf("Accuracy:  %.4f\n", matrix->accuracy);
    printf("==============================\n\n");
}

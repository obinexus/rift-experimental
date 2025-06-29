#ifndef DDN_CONFUSION_MATRIX_H
#define DDN_CONFUSION_MATRIX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Classification labels for QA metrics
typedef enum {
    DDN_TRUE_POSITIVE = 0,
    DDN_TRUE_NEGATIVE = 1,
    DDN_FALSE_POSITIVE = 2,
    DDN_FALSE_NEGATIVE = 3
} ddn_label_t;

typedef struct {
    uint32_t counts[4];
} ddn_confusion_matrix_t;

void ddn_cm_init(ddn_confusion_matrix_t* cm);
void ddn_cm_update(ddn_confusion_matrix_t* cm, ddn_label_t label);
uint32_t ddn_cm_get(const ddn_confusion_matrix_t* cm, ddn_label_t label);

#ifdef __cplusplus
}
#endif

#endif // DDN_CONFUSION_MATRIX_H

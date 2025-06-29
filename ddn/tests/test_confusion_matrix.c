#include "confusion_matrix.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    ddn_confusion_matrix_t cm;
    ddn_cm_init(&cm);

    ddn_cm_update(&cm, DDN_TRUE_POSITIVE);
    ddn_cm_update(&cm, DDN_TRUE_NEGATIVE);
    ddn_cm_update(&cm, DDN_FALSE_POSITIVE);
    ddn_cm_update(&cm, DDN_FALSE_NEGATIVE);

    assert(ddn_cm_get(&cm, DDN_TRUE_POSITIVE) == 1);
    assert(ddn_cm_get(&cm, DDN_TRUE_NEGATIVE) == 1);
    assert(ddn_cm_get(&cm, DDN_FALSE_POSITIVE) == 1);
    assert(ddn_cm_get(&cm, DDN_FALSE_NEGATIVE) == 1);

    printf("Confusion matrix tests passed.\n");
    return 0;
}

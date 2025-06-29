#include "confusion_matrix.h"
#include <string.h>

void ddn_cm_init(ddn_confusion_matrix_t* cm) {
    if (!cm) return;
    memset(cm, 0, sizeof(*cm));
}

void ddn_cm_update(ddn_confusion_matrix_t* cm, ddn_label_t label) {
    if (!cm || label < 0 || label > 3) return;
    cm->counts[label]++;
}

uint32_t ddn_cm_get(const ddn_confusion_matrix_t* cm, ddn_label_t label) {
    if (!cm || label < 0 || label > 3) return 0;
    return cm->counts[label];
}

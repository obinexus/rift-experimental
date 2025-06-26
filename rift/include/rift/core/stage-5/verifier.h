#ifndef RIFT_CORE_STAGE_5_VERIFIER_H
#define RIFT_CORE_STAGE_5_VERIFIER_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 5: Verifier Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 5
int rift_verifier_process(const void* input, void** output);
void rift_verifier_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_5_VERIFIER_H */

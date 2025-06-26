#ifndef RIFT_CORE_STAGE_3_VALIDATOR_H
#define RIFT_CORE_STAGE_3_VALIDATOR_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 3: Validator Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 3
int rift_validator_process(const void* input, void** output);
void rift_validator_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_3_VALIDATOR_H */

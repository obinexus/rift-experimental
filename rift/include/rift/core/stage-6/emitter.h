#ifndef RIFT_CORE_STAGE_6_EMITTER_H
#define RIFT_CORE_STAGE_6_EMITTER_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 6: Emitter Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 6
int rift_emitter_process(const void* input, void** output);
void rift_emitter_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_6_EMITTER_H */

#ifndef RIFT_CORE_STAGE_2_SEMANTIC_H
#define RIFT_CORE_STAGE_2_SEMANTIC_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 2: Semantic Analysis Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 2
int rift_semantic_process(const void* input, void** output);
void rift_semantic_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_2_SEMANTIC_H */

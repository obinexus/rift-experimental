#ifndef RIFT_CORE_STAGE_4_BYTECODE_H
#define RIFT_CORE_STAGE_4_BYTECODE_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 4: Bytecode Generator Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 4
int rift_bytecode_process(const void* input, void** output);
void rift_bytecode_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_4_BYTECODE_H */

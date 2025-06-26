#ifndef RIFT_CORE_STAGE_0_TOKENIZER_H
#define RIFT_CORE_STAGE_0_TOKENIZER_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 0: Tokenization Engine Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 0
int rift_tokenizer_process(const void* input, void** output);
void rift_tokenizer_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_0_TOKENIZER_H */

#ifndef RIFT_CORE_STAGE_1_PARSER_H
#define RIFT_CORE_STAGE_1_PARSER_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Stage 1: Parser Engine Header
// Version 1.0.0 - Build System Validation

// Function prototypes for stage 1
int rift_parser_process(const void* input, void** output);
void rift_parser_cleanup(void* data);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_1_PARSER_H */

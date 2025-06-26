#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rift/core/common.h"

int rift_semantic_process(const void* input, void** output) {
    if (!input || !output) return RIFT_ERROR_INVALID_ARGUMENT;
    *output = malloc(1024);
    return *output ? RIFT_SUCCESS : RIFT_ERROR_MEMORY_ALLOCATION;
}

void rift_semantic_cleanup(void* data) {
    if (data) free(data);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rift/core/common.h"

int rift_tokenizer_process(const char* input, void** tokens) {
    if (!input || !tokens) return RIFT_ERROR_INVALID_ARGUMENT;
    *tokens = malloc(1024);
    return *tokens ? RIFT_SUCCESS : RIFT_ERROR_MEMORY_ALLOCATION;
}

void rift_tokenizer_cleanup(void* tokens) {
    if (tokens) free(tokens);
}

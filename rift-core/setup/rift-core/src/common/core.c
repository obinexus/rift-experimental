#include "rift-core/core.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool rift_initialized = false;

rift_result_t rift_core_init(void) {
    if (rift_initialized) {
        return RIFT_SUCCESS;
    }
    
    /* Initialize random number generator */
    srand((unsigned int)time(NULL));
    
    rift_initialized = true;
    return RIFT_SUCCESS;
}

void rift_core_cleanup(void) {
    rift_initialized = false;
}

rift_result_t rift_context_create(rift_context_t *ctx) {
    if (RIFT_YODA_EQ(RIFT_NIL, ctx)) {
        return RIFT_ERROR_BASIC;
    }
    
    /* Generate unique ID */
    ctx->id = (uint64_t)time(NULL) * 1000 + (rand() % 1000);
    
    /* Generate UUID placeholder */
    snprintf(ctx->uuid, sizeof(ctx->uuid), 
             "%08x-%04x-%04x-%04x-%012x",
             rand(), rand() & 0xFFFF, rand() & 0xFFFF,
             rand() & 0xFFFF, rand());
    
    /* Generate hash placeholder */
    snprintf(ctx->hash, sizeof(ctx->hash),
             "%064x", rand());
    
    /* Initialize PRNG seed */
    ctx->prng_seed = (uint32_t)time(NULL);
    
    return RIFT_SUCCESS;
}

void rift_context_destroy(rift_context_t *ctx) {
    if (RIFT_YODA_NE(RIFT_NIL, ctx)) {
        memset(ctx, 0, sizeof(rift_context_t));
    }
}

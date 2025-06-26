#ifndef RIFT_AUDIT_TRACER_H
#define RIFT_AUDIT_TRACER_H

#include "../core.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIT_STDIN = 0,
    AUDIT_STDERR = 1,
    AUDIT_STDOUT = 2
} audit_stream_t;

typedef struct {
    audit_stream_t stream;
    char filename[256];
    FILE *file;
    char state_hash[65];
    rift_context_t context;
} rift_audit_t;

/* Audit file management */
rift_result_t rift_audit_init(rift_audit_t *audit, audit_stream_t stream, int stage);
rift_result_t rift_audit_write(rift_audit_t *audit, const char *data, size_t size);
rift_result_t rift_audit_finalize(rift_audit_t *audit);
void rift_audit_cleanup(rift_audit_t *audit);

/* Audit utilities */
rift_result_t rift_audit_generate_hash(const char *data, size_t size, char *hash_out);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_AUDIT_TRACER_H */

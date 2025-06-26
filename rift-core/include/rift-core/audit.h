#ifndef RIFT_AUDIT_H
#define RIFT_AUDIT_H

#include "core.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT Audit Stream Types */
typedef enum {
    RIFT_AUDIT_STDIN = 0,   /* .audit-0 */
    RIFT_AUDIT_STDERR = 1,  /* .audit-1 */
    RIFT_AUDIT_STDOUT = 2   /* .audit-2 */
} rift_audit_stream_t;

/* RIFT Audit Context */
typedef struct {
    rift_audit_stream_t stream;
    char audit_filename[256];
    FILE *audit_file;
    char state_hash[65];
    rift_context_t context;
    uint64_t audit_sequence;
} rift_audit_t;

/* RIFT Audit Management Functions */
rift_result_t rift_audit_init(rift_audit_t *audit, rift_audit_stream_t stream, int stage);
rift_result_t rift_audit_write(rift_audit_t *audit, const char *data, size_t size);
rift_result_t rift_audit_write_colored(rift_audit_t *audit, const char *level, 
                                      const char *data, size_t size);
rift_result_t rift_audit_finalize(rift_audit_t *audit);
void rift_audit_cleanup(rift_audit_t *audit);

/* RIFT Audit Utilities */
rift_result_t rift_audit_generate_hash(const char *data, size_t size, char *hash_out);
rift_result_t rift_audit_verify_integrity(const rift_audit_t *audit);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_AUDIT_H */

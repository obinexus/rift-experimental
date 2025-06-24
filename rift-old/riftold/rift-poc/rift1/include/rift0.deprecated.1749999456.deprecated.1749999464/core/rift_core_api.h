#ifndef RIFT_CORE_API_H
#define RIFT_CORE_API_H

// OBINexus RIFT Core API
// Clean boundary between core engine and CLI interface

#include "r_extension/r_extensions.h"
#include "rift.h"

// === CORE ENGINE INTERFACE ===
typedef struct rift_engine rift_engine_t;

// Engine lifecycle
rift_engine_t *rift_engine_create(void);
void rift_engine_destroy(rift_engine_t *engine);

// Compilation pipeline
int rift_compile_source(rift_engine_t *engine, const char *source_file,
                        const char *output_file);
int rift_validate_governance(rift_engine_t *engine, const char *policy_file);

// R extension integration
int rift_register_r_extension(rift_engine_t *engine, r_extension_t *extension);
int rift_execute_r_chain(rift_engine_t *engine, void *input,
                         const char *chain_spec);

// Governance validation
bool rift_check_governance_compliance(rift_engine_t *engine);
float rift_get_governance_score(rift_engine_t *engine);

#endif // RIFT_CORE_API_H

/*
 * PoLiC: Security Framework for Sandboxed Environments
 * OBINexus Computing - Zero Trust Architecture
 */

#ifndef RIFT_POLIC_H
#define RIFT_POLIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== PoLiC Policy Types ===== */
typedef enum {
    POLIC_ALLOW = 0,
    POLIC_BLOCK = 1,
    POLIC_LOG_ONLY = 2
} PoliC_Action;

typedef enum {
    POLIC_SANDBOX_ON = 1,
    POLIC_SANDBOX_OFF = 0
} PoliC_SandboxMode;

/* ===== PoLiC Function Decorators ===== */
#define POLIC_DECORATOR(func) polic_secure_call((void*)func, #func)

/* ===== PoLiC API ===== */
int polic_init(bool sandbox_mode, PoliC_Action default_action);
void polic_cleanup(void);
void* polic_secure_call(void* function_ptr, const char* function_name);
bool polic_validate_execution_context(void);
int polic_enforce_stack_protection(void);
int polic_activate_vm_hooks(void);

/* ===== PoLiC Policy Management ===== */
int polic_set_policy(const char* function_name, PoliC_Action action);
PoliC_Action polic_get_policy(const char* function_name);
bool polic_is_sandboxed(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_POLIC_H */

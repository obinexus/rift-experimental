#ifndef RIFT_R_EXTENSIONS_H
#define RIFT_R_EXTENSIONS_H

#include "rift.h"
#include <stdbool.h>
#include <stdint.h>

// OBINexus RIFT R Syntax Extensions
// Governance-first functional composition and UML modeling

// === BOOLEAN LOGIC MACROS ===
#define R_AND(a, b) ((a) & (b))
#define R_OR(a, b) ((a) | (b))
#define R_XOR(a, b) ((a) ^ (b))
#define R_NOT(a) (~(a))
#define R_NAND(a, b) R_NOT(R_AND(a, b))
#define R_NOR(a, b) R_NOT(R_OR(a, b))
#define R_AND3(a, b, c) R_AND(R_AND(a, b), c)
#define R_OR3(a, b, c) R_OR(R_OR(a, b), c)

// === FUNCTIONAL COMPOSITION ===
#define R_COMPOSE(f, g) (lambda(x) { return f(g(x)); })
#define R_PIPE(x, f) (f(x))
#define R_CHAIN(input, ...) (chain_apply(input, ##__VA_ARGS__, NULL))

// === R EXTENSION SYSTEM ===
typedef struct {
  void *(*extend_func)(void *, void *);
  void *context;
  bool governance_validated;
  uint32_t governance_score;
} r_extension_t;

typedef struct {
  r_extension_t *extension;
  void *async_context;
  bool (*validation_callback)(void *);
} r_async_extension_t;

// R.extend - Async extension with governance validation
#define R_extend(ptr, ext_func)                                                \
  r_async_extend(&(ptr), ext_func, __FILE__, __LINE__)

// Function declarations
void *r_async_extend(void **target_ptr, void *(*extension_func)(void *, void *),
                     const char *file, int line);
bool r_validate_governance_triangle(r_extension_t *ext);
void r_extension_cleanup(r_extension_t *ext);

// Include sub-modules
#include "compose/r_composition.h"
#include "governance/r_governance_validation.h"
#include "logic/r_logic_gates.h"
#include "uml/r_uml_patterns.h"

#endif // RIFT_R_EXTENSIONS_H

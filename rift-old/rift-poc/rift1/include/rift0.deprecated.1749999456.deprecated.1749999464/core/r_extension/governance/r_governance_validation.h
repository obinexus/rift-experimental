#ifndef RIFT_R_GOVERNANCE_VALIDATION_H
#define RIFT_R_GOVERNANCE_VALIDATION_H

#include <stdbool.h>
#include <stdint.h>

// OBINexus RIFT Governance Triangle Validation for R Extensions
// Implements NASA-STD-8739.8 compliance through mathematical determinism

// === GOVERNANCE TRIANGLE THRESHOLDS ===
#define GOVERNANCE_THRESHOLD_MAX 0.5f
#define ATTACK_RISK_MAX 0.2f
#define ROLLBACK_COST_MAX 0.2f
#define STABILITY_IMPACT_MAX 0.1f

// === GOVERNANCE TRIANGLE STRUCTURE ===
typedef struct {
  float attack_risk;      // a: Attack Risk Plane measurement
  float rollback_cost;    // r: Rollback Cost Plane measurement
  float stability_impact; // s: Stability Impact Plane measurement
} governance_triangle_t;

typedef enum {
  GOVERNANCE_APPROVED,
  GOVERNANCE_WARNING,
  GOVERNANCE_REJECTED
} governance_result_t;

// === GOVERNANCE VALIDATION FUNCTIONS ===
governance_result_t
validate_governance_triangle(governance_triangle_t *triangle);
float calculate_governance_norm(governance_triangle_t *triangle);
governance_triangle_t *evaluate_r_extension_governance(void *extension_context);
bool is_governance_compliant(governance_triangle_t *triangle);

// === GOVERNANCE MACROS ===
#define GOVERNANCE_VALIDATE(ext)                                               \
  validate_governance_triangle(evaluate_r_extension_governance(ext))
#define GOVERNANCE_NORM(triangle)                                              \
  ((triangle)->attack_risk + (triangle)->rollback_cost +                       \
   (triangle)->stability_impact)

#endif // RIFT_R_GOVERNANCE_VALIDATION_H

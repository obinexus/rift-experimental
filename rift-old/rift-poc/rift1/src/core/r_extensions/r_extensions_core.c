#include "rift1/core/rift.h"
#include <stdio.h>
#include <stdlib.h>

// OBINexus RIFT R Extensions Core Implementation
// Provides async pointer extension with governance validation

void *r_async_extend(void **target_ptr, void *(*extension_func)(void *, void *),
                     const char *file, int line) {
  if (!target_ptr || !extension_func) {
    fprintf(stderr, "R.extend error: Invalid parameters at %s:%d\n", file,
            line);
    return NULL;
  }

  // Create extension context
  r_extension_t *ext = malloc(sizeof(r_extension_t));
  if (!ext) {
    fprintf(stderr, "R.extend error: Memory allocation failed at %s:%d\n", file,
            line);
    return NULL;
  }

  ext->extend_func = extension_func;
  ext->context = *target_ptr;
  ext->governance_validated = false;
  ext->governance_score = 0;

  // Perform governance validation
  if (r_validate_governance_triangle(ext)) {
    ext->governance_validated = true;

    // Execute extension with governance approval
    void *result = ext->extend_func(*target_ptr, ext->context);

    // Update target pointer with result
    *target_ptr = result;

    printf("R.extend: Governance-approved extension at %s:%d\n", file, line);
    return result;
  } else {
    printf("R.extend: Governance violation - extension rejected at %s:%d\n",
           file, line);
    free(ext);
    return NULL;
  }
}

bool r_validate_governance_triangle(r_extension_t *ext) {
  if (!ext)
    return false;

  // Evaluate governance triangle for extension
  governance_triangle_t triangle = {
      .attack_risk = 0.05f,     // Low risk for pointer extensions
      .rollback_cost = 0.1f,    // Moderate rollback cost
      .stability_impact = 0.05f // Low stability impact
  };

  // Calculate governance norm
  float norm = GOVERNANCE_NORM(&triangle);
  ext->governance_score = (uint32_t)(norm * 1000); // Store as integer

  // Check compliance
  return is_governance_compliant(&triangle);
}

void r_extension_cleanup(r_extension_t *ext) {
  if (ext) {
    free(ext);
  }
}

// include/rift1/token_values/token_value.h - RIFT1 Token Value Engine
// OBINexus AEGIS Framework - Type-safe value resolution with compile-time
// validation
#ifndef RIFT1_TOKEN_VALUE_H
#define RIFT1_TOKEN_VALUE_H

#include "../lexer/lexer.h"
#include "../token_types/token_type.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Value Union - Type-safe value storage
typedef union {
  // Numeric types
  int64_t integer_value;
  double float_value;

  // String types
  struct {
    char *data;
    size_t length;
    size_t capacity;
  } string_value;

  // Character type
  char char_value;

  // Boolean type
  bool boolean_value;

  // Quantum superposition value (experimental)
  struct {
    double *probability_amplitudes;
    size_t state_count;
    bool is_entangled;
  } quantum_value;

  // Raw data for unknown types
  struct {
    void *data;
    size_t size;
  } raw_value;
} RiftValue;

// Value metadata for AEGIS compliance
typedef struct {
  RiftTokenType type;
  bool is_valid;
  bool is_constant;
  bool is_quantum_superposition;
  size_t precision;     // For numeric types
  const char *encoding; // For string types

  // Compile-time safety metadata
  bool type_validated;
  bool value_validated;
  bool bounds_checked;
  const char *validation_error;
} RiftValueMetadata;

// Combined token value structure
typedef struct {
  RiftValue value;
  RiftValueMetadata metadata;
  RiftLexeme *source_lexeme; // Reference to original lexeme

  // AEGIS state tracking
  size_t matched_state;
  bool stage0_validated;
  bool stage1_enhanced;
} RiftTokenValue;

// Value resolver configuration
typedef struct {
  bool strict_type_checking;
  bool quantum_mode_enabled;
  bool auto_type_promotion;
  size_t max_string_length;
  int numeric_precision;
} RiftValueResolverConfig;

// Value resolver engine
typedef struct {
  RiftValueResolverConfig config;
  RiftTokenTypeMatcher *type_matcher;

  // Validation callbacks
  bool (*custom_validator)(const RiftTokenValue *value);
  void (*error_handler)(const char *error_message);
} RiftValueResolver;

// Core value resolution operations
RiftValueResolver *
rift_value_resolver_create(const RiftValueResolverConfig *config);
void rift_value_resolver_destroy(RiftValueResolver *resolver);

// Value resolution pipeline
RiftTokenValue *rift_value_resolve(RiftValueResolver *resolver,
                                   const RiftLexeme *lexeme,
                                   RiftTokenType type);
bool rift_value_validate(const RiftTokenValue *token_value);

// Type-specific value parsing
bool rift_value_parse_integer(const char *text, size_t length, int64_t *result);
bool rift_value_parse_float(const char *text, size_t length, double *result);
bool rift_value_parse_string(const char *text, size_t length, char **result,
                             size_t *result_length);
bool rift_value_parse_boolean(const char *text, size_t length, bool *result);
bool rift_value_parse_char(const char *text, size_t length, char *result);

// Quantum value operations (experimental)
bool rift_value_create_quantum_superposition(RiftTokenValue *token_value,
                                             double *amplitudes,
                                             size_t state_count);
bool rift_value_collapse_quantum_state(RiftTokenValue *token_value);

// Value utilities
RiftTokenValue *rift_token_value_create(RiftTokenType type);
void rift_token_value_destroy(RiftTokenValue *token_value);
RiftTokenValue *rift_token_value_copy(const RiftTokenValue *token_value);

// Value comparison and operations
bool rift_value_equals(const RiftTokenValue *a, const RiftTokenValue *b);
int rift_value_compare(const RiftTokenValue *a, const RiftTokenValue *b);

// Compile-time safety validation
bool rift_value_check_type_safety(const RiftTokenValue *token_value);
bool rift_value_check_bounds(const RiftTokenValue *token_value);
const char *rift_value_get_validation_error(const RiftTokenValue *token_value);

// String representations
char *rift_value_to_string(const RiftTokenValue *token_value);
bool rift_value_from_string(RiftTokenValue *token_value, const char *str);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKEN_VALUE_H

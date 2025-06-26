// src/core/token_values/token_value.c - RIFT1 Token Value Implementation
// OBINexus AEGIS Framework - Type-safe value resolution engine
#include "../../../include/rift1/token_values/token_value.h"
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default value resolver configuration
static const RiftValueResolverConfig DEFAULT_CONFIG = {
    .strict_type_checking = true,
    .quantum_mode_enabled = false,
    .auto_type_promotion = false,
    .max_string_length = 4096,
    .numeric_precision = 16};

RiftValueResolver *
rift_value_resolver_create(const RiftValueResolverConfig *config) {
  RiftValueResolver *resolver = calloc(1, sizeof(RiftValueResolver));
  if (!resolver)
    return NULL;

  // Use provided config or default
  resolver->config = config ? *config : DEFAULT_CONFIG;

  // Create type matcher for value resolution
  resolver->type_matcher = rift_token_type_matcher_create();
  if (!resolver->type_matcher) {
    free(resolver);
    return NULL;
  }

  return resolver;
}

void rift_value_resolver_destroy(RiftValueResolver *resolver) {
  if (!resolver)
    return;

  if (resolver->type_matcher) {
    rift_token_type_matcher_destroy(resolver->type_matcher);
  }

  free(resolver);
}

RiftTokenValue *rift_value_resolve(RiftValueResolver *resolver,
                                   const RiftLexeme *lexeme,
                                   RiftTokenType type) {
  if (!resolver || !lexeme || !lexeme->text)
    return NULL;

  // Create token value structure
  RiftTokenValue *token_value = rift_token_value_create(type);
  if (!token_value)
    return NULL;

  // Store reference to source lexeme
  token_value->source_lexeme = rift_lexeme_copy(lexeme);

  // Initialize metadata
  token_value->metadata.type = type;
  token_value->metadata.is_valid = false;
  token_value->metadata.type_validated = true; // Type already determined

  // Resolve value based on type
  bool resolution_success = false;

  switch (type) {
  case RIFT_TOKEN_INTEGER:
    resolution_success = rift_value_parse_integer(
        lexeme->text, lexeme->length, &token_value->value.integer_value);
    break;

  case RIFT_TOKEN_FLOAT:
    resolution_success = rift_value_parse_float(
        lexeme->text, lexeme->length, &token_value->value.float_value);
    break;

  case RIFT_TOKEN_STRING:
    resolution_success = rift_value_parse_string(
        lexeme->text, lexeme->length, &token_value->value.string_value.data,
        &token_value->value.string_value.length);
    if (resolution_success) {
      token_value->value.string_value.capacity =
          token_value->value.string_value.length + 1;
    }
    break;

  case RIFT_TOKEN_CHAR:
    resolution_success = rift_value_parse_char(lexeme->text, lexeme->length,
                                               &token_value->value.char_value);
    break;

  case RIFT_TOKEN_BOOLEAN:
    resolution_success = rift_value_parse_boolean(
        lexeme->text, lexeme->length, &token_value->value.boolean_value);
    break;

  case RIFT_TOKEN_IDENTIFIER:
  case RIFT_TOKEN_KEYWORD:
    // Store as string for identifiers and keywords
    resolution_success = rift_value_parse_string(
        lexeme->text, lexeme->length, &token_value->value.string_value.data,
        &token_value->value.string_value.length);
    if (resolution_success) {
      token_value->value.string_value.capacity =
          token_value->value.string_value.length + 1;
    }
    break;

  default:
    // Store as raw data for unknown types
    token_value->value.raw_value.size = lexeme->length;
    token_value->value.raw_value.data = malloc(lexeme->length);
    if (token_value->value.raw_value.data) {
      memcpy(token_value->value.raw_value.data, lexeme->text, lexeme->length);
      resolution_success = true;
    }
    break;
  }

  // Update metadata based on resolution result
  token_value->metadata.is_valid = resolution_success;
  token_value->metadata.value_validated = resolution_success;

  if (!resolution_success) {
    char error_buffer[256];
    snprintf(error_buffer, sizeof(error_buffer),
             "Failed to resolve value for type %s from lexeme '%.*s'",
             rift_token_type_name(type), (int)lexeme->length, lexeme->text);
    token_value->metadata.validation_error = strdup(error_buffer);
  }

  return token_value;
}

// Type-specific parsing implementations
bool rift_value_parse_integer(const char *text, size_t length,
                              int64_t *result) {
  if (!text || !result || length == 0)
    return false;

  // Create null-terminated string for parsing
  char *temp_str = malloc(length + 1);
  if (!temp_str)
    return false;

  memcpy(temp_str, text, length);
  temp_str[length] = '\0';

  errno = 0;
  char *endptr;
  long long value = strtoll(temp_str, &endptr, 10);

  bool success = (errno == 0 && endptr == temp_str + length &&
                  value >= INT64_MIN && value <= INT64_MAX);

  if (success) {
    *result = (int64_t)value;
  }

  free(temp_str);
  return success;
}

bool rift_value_parse_float(const char *text, size_t length, double *result) {
  if (!text || !result || length == 0)
    return false;

  char *temp_str = malloc(length + 1);
  if (!temp_str)
    return false;

  memcpy(temp_str, text, length);
  temp_str[length] = '\0';

  errno = 0;
  char *endptr;
  double value = strtod(temp_str, &endptr);

  bool success = (errno == 0 && endptr == temp_str + length);

  if (success) {
    *result = value;
  }

  free(temp_str);
  return success;
}

bool rift_value_parse_string(const char *text, size_t length, char **result,
                             size_t *result_length) {
  if (!text || !result || !result_length || length < 2)
    return false;

  // Check for string delimiters
  if (text[0] != '"' || text[length - 1] != '"')
    return false;

  // Extract string content (without quotes)
  size_t content_length = length - 2;
  char *string_content = malloc(content_length + 1);
  if (!string_content)
    return false;

  memcpy(string_content, text + 1, content_length);
  string_content[content_length] = '\0';

  // TODO: Handle escape sequences

  *result = string_content;
  *result_length = content_length;
  return true;
}

bool rift_value_parse_boolean(const char *text, size_t length, bool *result) {
  if (!text || !result)
    return false;

  if (length == 4 && strncmp(text, "true", 4) == 0) {
    *result = true;
    return true;
  } else if (length == 5 && strncmp(text, "false", 5) == 0) {
    *result = false;
    return true;
  }

  return false;
}

bool rift_value_parse_char(const char *text, size_t length, char *result) {
  if (!text || !result || length != 3)
    return false;

  // Check for character delimiters: 'c'
  if (text[0] != '\'' || text[2] != '\'')
    return false;

  *result = text[1];
  return true;
}

// Token value management
RiftTokenValue *rift_token_value_create(RiftTokenType type) {
  RiftTokenValue *token_value = calloc(1, sizeof(RiftTokenValue));
  if (!token_value)
    return NULL;

  token_value->metadata.type = type;
  token_value->metadata.is_valid = false;
  token_value->metadata.is_constant = false;
  token_value->metadata.is_quantum_superposition = false;

  return token_value;
}

void rift_token_value_destroy(RiftTokenValue *token_value) {
  if (!token_value)
    return;

  // Cleanup value data based on type
  switch (token_value->metadata.type) {
  case RIFT_TOKEN_STRING:
  case RIFT_TOKEN_IDENTIFIER:
  case RIFT_TOKEN_KEYWORD:
    if (token_value->value.string_value.data) {
      free(token_value->value.string_value.data);
    }
    break;

  default:
    if (token_value->value.raw_value.data) {
      free(token_value->value.raw_value.data);
    }
    break;
  }

  // Cleanup quantum data if present
  if (token_value->metadata.is_quantum_superposition &&
      token_value->value.quantum_value.probability_amplitudes) {
    free(token_value->value.quantum_value.probability_amplitudes);
  }

  // Cleanup source lexeme
  if (token_value->source_lexeme) {
    rift_lexeme_destroy(token_value->source_lexeme);
    free(token_value->source_lexeme);
  }

  // Cleanup validation error message
  if (token_value->metadata.validation_error) {
    free((char *)token_value->metadata.validation_error);
  }

  free(token_value);
}

bool rift_value_validate(const RiftTokenValue *token_value) {
  if (!token_value)
    return false;

  // Check type validation
  if (!token_value->metadata.type_validated)
    return false;

  // Check value validation
  if (!token_value->metadata.value_validated)
    return false;

  // Check bounds for numeric types
  switch (token_value->metadata.type) {
  case RIFT_TOKEN_INTEGER:
    // Integer bounds already checked during parsing
    break;

  case RIFT_TOKEN_FLOAT:
    // Check for infinity and NaN
    if (!isfinite(token_value->value.float_value))
      return false;
    break;

  case RIFT_TOKEN_STRING:
  case RIFT_TOKEN_IDENTIFIER:
  case RIFT_TOKEN_KEYWORD:
    // Check string length bounds
    if (token_value->value.string_value.length > 4096)
      return false;
    break;

  default:
    break;
  }

  return token_value->metadata.is_valid;
}

// Compile-time safety validation
bool rift_value_check_type_safety(const RiftTokenValue *token_value) {
  if (!token_value)
    return false;

  // Verify type and value consistency
  switch (token_value->metadata.type) {
  case RIFT_TOKEN_INTEGER:
    return token_value->metadata.value_validated;

  case RIFT_TOKEN_FLOAT:
    return token_value->metadata.value_validated &&
           isfinite(token_value->value.float_value);

  case RIFT_TOKEN_STRING:
  case RIFT_TOKEN_IDENTIFIER:
  case RIFT_TOKEN_KEYWORD:
    return token_value->value.string_value.data != NULL &&
           token_value->value.string_value.length > 0;

  case RIFT_TOKEN_BOOLEAN:
    return token_value->metadata.value_validated;

  case RIFT_TOKEN_CHAR:
    return token_value->metadata.value_validated;

  default:
    return false;
  }
}

const char *rift_value_get_validation_error(const RiftTokenValue *token_value) {
  return token_value ? token_value->metadata.validation_error
                     : "Invalid token value";
}

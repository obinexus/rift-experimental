// src/core/token_types/token_type.c - RIFT1 Token Type Implementation
// OBINexus AEGIS Framework - Pattern-based token classification engine
#include "../../../include/rift1/token_types/token_type.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Default token patterns for RIFT language
static const RiftTokenPattern DEFAULT_PATTERNS[] = {
    // High-priority exact matches
    {RIFT_TOKEN_KEYWORD, "if", "Conditional keyword", false, 100},
    {RIFT_TOKEN_KEYWORD, "else", "Else keyword", false, 100},
    {RIFT_TOKEN_KEYWORD, "while", "While loop keyword", false, 100},
    {RIFT_TOKEN_KEYWORD, "for", "For loop keyword", false, 100},
    {RIFT_TOKEN_KEYWORD, "return", "Return keyword", false, 100},
    {RIFT_TOKEN_KEYWORD, "function", "Function keyword", false, 100},

    // RIFT-specific keywords
    {RIFT_TOKEN_CLASSICAL_MODE, "classical", "Classical mode directive", false,
     95},
    {RIFT_TOKEN_QUANTUM_MODE, "quantum", "Quantum mode directive", false, 95},
    {RIFT_TOKEN_RIFT_DIRECTIVE, "@rift", "RIFT directive", false, 95},

    // Boolean literals
    {RIFT_TOKEN_BOOLEAN, "true", "Boolean true", false, 90},
    {RIFT_TOKEN_BOOLEAN, "false", "Boolean false", false, 90},

    // Operators (exact matches first)
    {RIFT_TOKEN_OPERATOR_EQUAL, "==", "Equality operator", false, 85},
    {RIFT_TOKEN_OPERATOR_NOT_EQUAL, "!=", "Not equal operator", false, 85},
    {RIFT_TOKEN_OPERATOR_LESS_EQUAL, "<=", "Less or equal operator", false, 85},
    {RIFT_TOKEN_OPERATOR_GREATER_EQUAL, ">=", "Greater or equal operator",
     false, 85},
    {RIFT_TOKEN_OPERATOR_AND, "&&", "Logical AND operator", false, 85},
    {RIFT_TOKEN_OPERATOR_OR, "||", "Logical OR operator", false, 85},

    // Single character operators
    {RIFT_TOKEN_OPERATOR_ASSIGN, "=", "Assignment operator", false, 80},
    {RIFT_TOKEN_OPERATOR_PLUS, "+", "Addition operator", false, 80},
    {RIFT_TOKEN_OPERATOR_MINUS, "-", "Subtraction operator", false, 80},
    {RIFT_TOKEN_OPERATOR_MULTIPLY, "*", "Multiplication operator", false, 80},
    {RIFT_TOKEN_OPERATOR_DIVIDE, "/", "Division operator", false, 80},
    {RIFT_TOKEN_OPERATOR_MODULO, "%", "Modulo operator", false, 80},
    {RIFT_TOKEN_OPERATOR_LESS, "<", "Less than operator", false, 80},
    {RIFT_TOKEN_OPERATOR_GREATER, ">", "Greater than operator", false, 80},
    {RIFT_TOKEN_OPERATOR_NOT, "!", "Logical NOT operator", false, 80},

    // Delimiters
    {RIFT_TOKEN_DELIMITER_SEMICOLON, ";", "Semicolon", false, 75},
    {RIFT_TOKEN_DELIMITER_COMMA, ",", "Comma", false, 75},
    {RIFT_TOKEN_DELIMITER_DOT, ".", "Dot", false, 75},
    {RIFT_TOKEN_DELIMITER_COLON, ":", "Colon", false, 75},

    // Brackets
    {RIFT_TOKEN_BRACKET_OPEN_PAREN, "(", "Open parenthesis", false, 75},
    {RIFT_TOKEN_BRACKET_CLOSE_PAREN, ")", "Close parenthesis", false, 75},
    {RIFT_TOKEN_BRACKET_OPEN_BRACE, "{", "Open brace", false, 75},
    {RIFT_TOKEN_BRACKET_CLOSE_BRACE, "}", "Close brace", false, 75},
    {RIFT_TOKEN_BRACKET_OPEN_SQUARE, "[", "Open square bracket", false, 75},
    {RIFT_TOKEN_BRACKET_CLOSE_SQUARE, "]", "Close square bracket", false, 75},

    // Pattern-based matches (lower priority)
    {RIFT_TOKEN_INTEGER, "^[0-9]+$", "Integer literal", true, 50},
    {RIFT_TOKEN_FLOAT, "^[0-9]+\\.[0-9]+$", "Float literal", true, 50},
    {RIFT_TOKEN_STRING, "^\".*\"$", "String literal", true, 50},
    {RIFT_TOKEN_CHAR, "^'.'$", "Character literal", true, 50},
    {RIFT_TOKEN_IDENTIFIER, "^[a-zA-Z_][a-zA-Z0-9_]*$", "Identifier", true, 10},
};

RiftTokenTypeMatcher *rift_token_type_matcher_create(void) {
  RiftTokenTypeMatcher *matcher = calloc(1, sizeof(RiftTokenTypeMatcher));
  if (!matcher)
    return NULL;

  // Initialize with default patterns
  size_t default_count = sizeof(DEFAULT_PATTERNS) / sizeof(RiftTokenPattern);
  matcher->pattern_capacity = default_count * 2; // Room for growth
  matcher->patterns =
      calloc(matcher->pattern_capacity, sizeof(RiftTokenPattern));

  if (!matcher->patterns) {
    free(matcher);
    return NULL;
  }

  // Copy default patterns
  for (size_t i = 0; i < default_count; i++) {
    matcher->patterns[i] = DEFAULT_PATTERNS[i];
    // Duplicate pattern strings for memory safety
    matcher->patterns[i].pattern = strdup(DEFAULT_PATTERNS[i].pattern);
    matcher->patterns[i].description = strdup(DEFAULT_PATTERNS[i].description);
  }
  matcher->pattern_count = default_count;

  matcher->case_sensitive = true;
  matcher->quantum_mode_enabled = false;

  return matcher;
}

void rift_token_type_matcher_destroy(RiftTokenTypeMatcher *matcher) {
  if (!matcher)
    return;

  if (matcher->patterns) {
    for (size_t i = 0; i < matcher->pattern_count; i++) {
      free((char *)matcher->patterns[i].pattern);
      free((char *)matcher->patterns[i].description);
    }
    free(matcher->patterns);
  }

  free(matcher);
}

RiftTokenType rift_token_type_match(const RiftTokenTypeMatcher *matcher,
                                    const RiftLexeme *lexeme) {
  if (!matcher || !lexeme || !lexeme->text) {
    return RIFT_TOKEN_UNKNOWN;
  }

  // Sort patterns by priority (highest first)
  // For now, iterate through patterns in order
  for (size_t i = 0; i < matcher->pattern_count; i++) {
    const RiftTokenPattern *pattern = &matcher->patterns[i];

    if (pattern->is_regex) {
      // Simple regex pattern matching
      if (rift_pattern_match(pattern->pattern, lexeme->text, lexeme->length)) {
        return pattern->type;
      }
    } else {
      // Exact string matching
      size_t pattern_len = strlen(pattern->pattern);
      if (pattern_len == lexeme->length) {
        int cmp_result;
        if (matcher->case_sensitive) {
          cmp_result = strncmp(pattern->pattern, lexeme->text, lexeme->length);
        } else {
          cmp_result =
              strncasecmp(pattern->pattern, lexeme->text, lexeme->length);
        }

        if (cmp_result == 0) {
          return pattern->type;
        }
      }
    }
  }

  return RIFT_TOKEN_UNKNOWN;
}

// Simplified regex pattern matching (basic implementation)
bool rift_pattern_match(const char *pattern, const char *text,
                        size_t text_length) {
  if (!pattern || !text)
    return false;

  // Handle simple patterns without full regex engine
  // Integer pattern: ^[0-9]+$
  if (strcmp(pattern, "^[0-9]+$") == 0) {
    for (size_t i = 0; i < text_length; i++) {
      if (!isdigit(text[i]))
        return false;
    }
    return text_length > 0;
  }

  // Float pattern: ^[0-9]+\.[0-9]+$
  if (strcmp(pattern, "^[0-9]+\\.[0-9]+$") == 0) {
    bool found_dot = false;
    bool has_digits_before = false;
    bool has_digits_after = false;

    for (size_t i = 0; i < text_length; i++) {
      if (text[i] == '.') {
        if (found_dot)
          return false; // Multiple dots
        found_dot = true;
      } else if (isdigit(text[i])) {
        if (!found_dot)
          has_digits_before = true;
        else
          has_digits_after = true;
      } else {
        return false; // Invalid character
      }
    }

    return found_dot && has_digits_before && has_digits_after;
  }

  // String pattern: ^".*"$
  if (strcmp(pattern, "^\".*\"$") == 0) {
    return text_length >= 2 && text[0] == '"' && text[text_length - 1] == '"';
  }

  // Character pattern: ^'.'$
  if (strcmp(pattern, "^'.'$") == 0) {
    return text_length == 3 && text[0] == '\'' && text[2] == '\'';
  }

  // Identifier pattern: ^[a-zA-Z_][a-zA-Z0-9_]*$
  if (strcmp(pattern, "^[a-zA-Z_][a-zA-Z0-9_]*$") == 0) {
    if (text_length == 0)
      return false;

    // First character must be letter or underscore
    if (!isalpha(text[0]) && text[0] != '_')
      return false;

    // Remaining characters must be alphanumeric or underscore
    for (size_t i = 1; i < text_length; i++) {
      if (!isalnum(text[i]) && text[i] != '_')
        return false;
    }
    return true;
  }

  return false;
}

const char *rift_token_type_name(RiftTokenType type) {
  switch (type) {
  case RIFT_TOKEN_UNKNOWN:
    return "UNKNOWN";
  case RIFT_TOKEN_INTEGER:
    return "INTEGER";
  case RIFT_TOKEN_FLOAT:
    return "FLOAT";
  case RIFT_TOKEN_STRING:
    return "STRING";
  case RIFT_TOKEN_CHAR:
    return "CHAR";
  case RIFT_TOKEN_BOOLEAN:
    return "BOOLEAN";
  case RIFT_TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case RIFT_TOKEN_KEYWORD:
    return "KEYWORD";
  case RIFT_TOKEN_OPERATOR_ASSIGN:
    return "ASSIGN";
  case RIFT_TOKEN_OPERATOR_PLUS:
    return "PLUS";
  case RIFT_TOKEN_OPERATOR_MINUS:
    return "MINUS";
  case RIFT_TOKEN_OPERATOR_MULTIPLY:
    return "MULTIPLY";
  case RIFT_TOKEN_OPERATOR_DIVIDE:
    return "DIVIDE";
  case RIFT_TOKEN_DELIMITER_SEMICOLON:
    return "SEMICOLON";
  case RIFT_TOKEN_BRACKET_OPEN_PAREN:
    return "OPEN_PAREN";
  case RIFT_TOKEN_BRACKET_CLOSE_PAREN:
    return "CLOSE_PAREN";
  case RIFT_TOKEN_CLASSICAL_MODE:
    return "CLASSICAL_MODE";
  case RIFT_TOKEN_QUANTUM_MODE:
    return "QUANTUM_MODE";
  case RIFT_TOKEN_RIFT_DIRECTIVE:
    return "RIFT_DIRECTIVE";
  case RIFT_TOKEN_EOF:
    return "EOF";
  default:
    return "UNKNOWN";
  }
}

bool rift_token_type_is_valid(RiftTokenType type) {
  return type >= 0 && type < RIFT_TOKEN_TYPE_COUNT;
}

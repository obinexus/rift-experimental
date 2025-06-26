// include/rift1/lexer/lexer.h - RIFT1 Lexer Component
// OBINexus AEGIS Framework - Lexical Analysis Engine
#ifndef RIFT1_LEXER_H
#define RIFT1_LEXER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Lexer Configuration
typedef struct {
  bool unicode_support;
  bool quantum_mode;
  size_t buffer_size;
  size_t max_lexeme_length;
} RiftLexerConfig;

// Position tracking for error reporting
typedef struct {
  size_t line;
  size_t column;
  size_t offset;
  const char *filename;
} RiftPosition;

// Lexeme structure - raw token without type interpretation
typedef struct {
  char *text;
  size_t length;
  RiftPosition position;
  bool is_complete;
  uint32_t unicode_codepoint; // For Unicode character support
} RiftLexeme;

// Lexer state machine
typedef struct {
  const char *input;
  size_t input_length;
  size_t current_pos;
  RiftPosition position;
  RiftLexerConfig config;

  // Lexeme buffer management
  RiftLexeme *lexeme_buffer;
  size_t buffer_capacity;
  size_t buffer_count;

  // State tracking
  bool at_eof;
  bool has_error;
  char *error_message;
} RiftLexer;

// Core lexer operations
RiftLexer *rift_lexer_create(const RiftLexerConfig *config);
void rift_lexer_destroy(RiftLexer *lexer);

// Input management
bool rift_lexer_set_input(RiftLexer *lexer, const char *input, size_t length);
bool rift_lexer_set_file_input(RiftLexer *lexer, const char *filename);

// Lexeme extraction
RiftLexeme *rift_lexer_next_lexeme(RiftLexer *lexer);
RiftLexeme *rift_lexer_peek_lexeme(RiftLexer *lexer);
bool rift_lexer_advance(RiftLexer *lexer);

// Position management
RiftPosition rift_lexer_get_position(const RiftLexer *lexer);
bool rift_lexer_at_end(const RiftLexer *lexer);

// Unicode support functions
uint32_t rift_lexer_read_unicode(RiftLexer *lexer);
bool rift_lexer_is_valid_unicode(uint32_t codepoint);

// Error handling
const char *rift_lexer_get_error(const RiftLexer *lexer);
void rift_lexer_clear_error(RiftLexer *lexer);

// Lexeme utilities
RiftLexeme *rift_lexeme_create(const char *text, size_t length,
                               RiftPosition position);
void rift_lexeme_destroy(RiftLexeme *lexeme);
RiftLexeme *rift_lexeme_copy(const RiftLexeme *lexeme);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_LEXER_H

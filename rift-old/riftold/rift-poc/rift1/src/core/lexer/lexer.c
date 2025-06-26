// src/core/lexer/lexer.c - RIFT1 Lexer Implementation
// OBINexus AEGIS Framework - Position-aware lexical analysis
#include "../../../include/rift1/lexer/lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default lexer configuration
static const RiftLexerConfig DEFAULT_CONFIG = {.unicode_support = true,
                                               .quantum_mode = false,
                                               .buffer_size = 1024,
                                               .max_lexeme_length = 512};

RiftLexer *rift_lexer_create(const RiftLexerConfig *config) {
  RiftLexer *lexer = calloc(1, sizeof(RiftLexer));
  if (!lexer)
    return NULL;

  // Use provided config or default
  lexer->config = config ? *config : DEFAULT_CONFIG;

  // Initialize lexeme buffer
  lexer->buffer_capacity = lexer->config.buffer_size;
  lexer->lexeme_buffer = calloc(lexer->buffer_capacity, sizeof(RiftLexeme));
  if (!lexer->lexeme_buffer) {
    free(lexer);
    return NULL;
  }

  // Initialize position tracking
  lexer->position.line = 1;
  lexer->position.column = 1;
  lexer->position.offset = 0;
  lexer->position.filename = NULL;

  return lexer;
}

void rift_lexer_destroy(RiftLexer *lexer) {
  if (!lexer)
    return;

  // Cleanup lexeme buffer
  if (lexer->lexeme_buffer) {
    for (size_t i = 0; i < lexer->buffer_count; i++) {
      rift_lexeme_destroy(&lexer->lexeme_buffer[i]);
    }
    free(lexer->lexeme_buffer);
  }

  // Cleanup error message
  if (lexer->error_message) {
    free(lexer->error_message);
  }

  free(lexer);
}

bool rift_lexer_set_input(RiftLexer *lexer, const char *input, size_t length) {
  if (!lexer || !input)
    return false;

  lexer->input = input;
  lexer->input_length = length;
  lexer->current_pos = 0;
  lexer->at_eof = false;

  // Reset position tracking
  lexer->position.line = 1;
  lexer->position.column = 1;
  lexer->position.offset = 0;

  // Clear previous lexemes
  lexer->buffer_count = 0;

  return true;
}

RiftLexeme *rift_lexer_next_lexeme(RiftLexer *lexer) {
  if (!lexer || lexer->at_eof)
    return NULL;

  // Skip whitespace
  while (lexer->current_pos < lexer->input_length &&
         isspace(lexer->input[lexer->current_pos])) {
    if (lexer->input[lexer->current_pos] == '\n') {
      lexer->position.line++;
      lexer->position.column = 1;
    } else {
      lexer->position.column++;
    }
    lexer->current_pos++;
    lexer->position.offset++;
  }

  // Check for EOF
  if (lexer->current_pos >= lexer->input_length) {
    lexer->at_eof = true;
    return NULL;
  }

  // Extract lexeme
  size_t start_pos = lexer->current_pos;
  RiftPosition start_position = lexer->position;

  // Simple lexeme extraction (extend for complex patterns)
  while (lexer->current_pos < lexer->input_length &&
         !isspace(lexer->input[lexer->current_pos])) {
    lexer->current_pos++;
    lexer->position.column++;
    lexer->position.offset++;
  }

  size_t lexeme_length = lexer->current_pos - start_pos;
  if (lexeme_length == 0)
    return NULL;

  // Create lexeme
  const char *lexeme_text = &lexer->input[start_pos];
  return rift_lexeme_create(lexeme_text, lexeme_length, start_position);
}

RiftPosition rift_lexer_get_position(const RiftLexer *lexer) {
  return lexer ? lexer->position : (RiftPosition){0, 0, 0, NULL};
}

bool rift_lexer_at_end(const RiftLexer *lexer) {
  return lexer ? lexer->at_eof : true;
}

// Unicode support implementation
uint32_t rift_lexer_read_unicode(RiftLexer *lexer) {
  if (!lexer || !lexer->config.unicode_support)
    return 0;

  // Basic UTF-8 decoding (simplified)
  if (lexer->current_pos >= lexer->input_length)
    return 0;

  unsigned char byte = lexer->input[lexer->current_pos];

  // ASCII character
  if (byte < 0x80) {
    lexer->current_pos++;
    lexer->position.column++;
    lexer->position.offset++;
    return byte;
  }

  // Multi-byte UTF-8 (simplified implementation)
  // TODO: Complete UTF-8 decoding
  lexer->current_pos++;
  lexer->position.column++;
  lexer->position.offset++;
  return byte;
}

bool rift_lexer_is_valid_unicode(uint32_t codepoint) {
  // Basic Unicode validation
  return codepoint <= 0x10FFFF && !(codepoint >= 0xD800 && codepoint <= 0xDFFF);
}

// Lexeme utility functions
RiftLexeme *rift_lexeme_create(const char *text, size_t length,
                               RiftPosition position) {
  RiftLexeme *lexeme = calloc(1, sizeof(RiftLexeme));
  if (!lexeme)
    return NULL;

  lexeme->text = malloc(length + 1);
  if (!lexeme->text) {
    free(lexeme);
    return NULL;
  }

  memcpy(lexeme->text, text, length);
  lexeme->text[length] = '\0';
  lexeme->length = length;
  lexeme->position = position;
  lexeme->is_complete = true;

  return lexeme;
}

void rift_lexeme_destroy(RiftLexeme *lexeme) {
  if (!lexeme)
    return;
  if (lexeme->text)
    free(lexeme->text);
  // Note: Don't free lexeme itself if it's part of an array
}

const char *rift_lexer_get_error(const RiftLexer *lexer) {
  return lexer ? lexer->error_message : "Invalid lexer";
}

void rift_lexer_clear_error(RiftLexer *lexer) {
  if (!lexer)
    return;
  if (lexer->error_message) {
    free(lexer->error_message);
    lexer->error_message = NULL;
  }
  lexer->has_error = false;
}

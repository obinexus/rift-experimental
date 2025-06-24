// include/rift1/rift_files/rift_format.h - RIFT File Format Specification
// OBINexus AEGIS Framework - Classical and Quantum mode file handling
#ifndef RIFT1_RIFT_FORMAT_H
#define RIFT1_RIFT_FORMAT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// RIFT file format version
#define RIFT_FORMAT_VERSION_MAJOR 1
#define RIFT_FORMAT_VERSION_MINOR 0
#define RIFT_FORMAT_VERSION_PATCH 0

// RIFT file magic numbers
#define RIFT0_MAGIC 0x52494654 // "RIFT" for .rift.0 files
#define RIFT1_MAGIC 0x52494631 // "RIF1" for .rift.1 files

// RIFT file modes
typedef enum {
  RIFT_MODE_CLASSICAL = 0,
  RIFT_MODE_QUANTUM = 1,
  RIFT_MODE_HYBRID = 2
} RiftFileMode;

// RIFT.0 file header (Stage 0 - Lexical data)
typedef struct {
  uint32_t magic; // RIFT0_MAGIC
  uint16_t version_major;
  uint16_t version_minor;
  uint16_t version_patch;
  uint16_t mode; // RiftFileMode
  uint64_t timestamp;
  uint32_t lexeme_count;
  uint32_t total_size;
  uint32_t checksum;
  char reserved[16];
} __attribute__((packed)) Rift0Header;

// RIFT.1 file header (Stage 1 - Processed tokens)
typedef struct {
  uint32_t magic; // RIFT1_MAGIC
  uint16_t version_major;
  uint16_t version_minor;
  uint16_t version_patch;
  uint16_t mode; // RiftFileMode
  uint64_t timestamp;
  uint32_t token_count;
  uint32_t validation_flags;
  uint32_t enhancement_flags;
  uint32_t total_size;
  uint32_t checksum;
  char reserved[12];
} __attribute__((packed)) Rift1Header;

// RIFT.0 lexeme record
typedef struct {
  uint32_t text_length;
  uint32_t line;
  uint32_t column;
  uint64_t offset;
  // Followed by null-terminated text data
} __attribute__((packed)) Rift0LexemeRecord;

// RIFT.1 token record
typedef struct {
  uint32_t type;       // RiftTokenType
  uint32_t value_type; // Value union discriminator
  uint32_t value_size;
  uint32_t source_lexeme_index;
  uint64_t matched_state;
  uint8_t validation_flags;
  uint8_t enhancement_flags;
  uint16_t reserved;
  // Followed by value data
} __attribute__((packed)) Rift1TokenRecord;

// RIFT file I/O operations
bool rift_file_save_rift0(const char *filename, RiftLexeme **lexemes,
                          size_t lexeme_count, RiftFileMode mode);
bool rift_file_load_rift0(const char *filename, RiftLexeme ***lexemes,
                          size_t *lexeme_count, RiftFileMode *mode);

bool rift_file_save_rift1(const char *filename, RiftMemoryToken **tokens,
                          size_t token_count, RiftFileMode mode);
bool rift_file_load_rift1(const char *filename, RiftMemoryToken ***tokens,
                          size_t *token_count, RiftFileMode *mode);

// RIFT file validation and integrity
bool rift_file_validate_rift0(const char *filename);
bool rift_file_validate_rift1(const char *filename);
uint32_t rift_file_calculate_checksum(const void *data, size_t size);

// RIFT file format utilities
const char *rift_file_mode_name(RiftFileMode mode);
bool rift_file_is_valid_rift0(const char *filename);
bool rift_file_is_valid_rift1(const char *filename);

// Quantum coherence file operations (experimental)
bool rift_file_save_quantum_coherence(const char *filename,
                                      const RiftTokenMemory *memory);
bool rift_file_load_quantum_coherence(const char *filename,
                                      RiftTokenMemory *memory);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_RIFT_FORMAT_H

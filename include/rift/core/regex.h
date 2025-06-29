/*
 * rift/include/rift/core/regex.h
 * RIFT Regular Expression Lifecycle Management Interface
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#ifndef RIFT_CORE_REGEX_H
#define RIFT_CORE_REGEX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Regular expression lifecycle states */
typedef enum rift_regex_lifecycle_state {
    R_NIL_START     = 0x00,
    R_EMPTY_START   = 0x01,
    R_PATTERN_INIT  = 0x02,
    R_COMPILE_PHASE = 0x03,
    R_VALIDATE_PHASE= 0x04,
    R_OPTIMIZE_PHASE= 0x05,
    R_READY_STATE   = 0x06,
    R_EXECUTE_PHASE = 0x07,
    R_MATCH_SUCCESS = 0x08,
    R_MATCH_FAILURE = 0x09,
    R_ERROR_STATE   = 0x0A,
    R_CLEANUP_PHASE = 0x0B,
    R_END_STATE     = 0x0C,
    R_EOF           = 0xFF
} rift_regex_lifecycle_t;

/* Chomsky hierarchy classification */
typedef enum rift_chomsky_type {
    CHOMSKY_TYPE_3 = 3,
    CHOMSKY_TYPE_2 = 2,
    CHOMSKY_TYPE_1 = 1,
    CHOMSKY_TYPE_0 = 0
} rift_chomsky_type_t;

/* Regular expression pattern structure */
typedef struct rift_regex_pattern {
    rift_chomsky_type_t     chomsky_type;
    rift_regex_lifecycle_t  lifecycle_state;
    const char*             pattern_string;
    uint32_t                flags;
    uint64_t                pattern_hash;
    void*                   automaton_state;
} rift_regex_pattern_t;

/* flag definitions */
#define RIFT_REGEX_GLOBAL       0x01
#define RIFT_REGEX_MULTILINE    0x02
#define RIFT_REGEX_IGNORECASE   0x04
#define RIFT_REGEX_TAINTED      0x80

/* parse standard flag strings like "gmi" */
uint32_t rift_regex_parse_flags(const char* flag_string);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_REGEX_H */

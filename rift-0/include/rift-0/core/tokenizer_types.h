/**
 * =================================================================
 * tokenizer_types.h - RIFT-0 Core Type Definitions (Recovery Base)
 * RIFT: RIFT Is a Flexible Translator
 * Component: Isolated struct and enum definitions for tokenizer subsystem
 * OBINexus Computing Framework - Stage 0 Implementation
 * Recovery from .bak files with structural integrity restoration
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_TYPES_H
#define RIFT_0_CORE_TOKENIZER_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#if defined(_WIN32) || defined(_WIN64)
// Windows platform: pthreads and stdatomic are not available by default
#include <windows.h>
typedef HANDLE pthread_mutex_t;
typedef DWORD pthread_t;
typedef int atomic_bool;
#define ATOMIC_VAR_INIT(value) (value)
#else
#include <pthread.h>
#include <stdatomic.h>
#endif

#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma rift_policy memory(aligned(4)) type(strict) value(static)

#ifndef _SSIZE_T_DEFINED
typedef long ssize_t;
#define _SSIZE_T_DEFINED
#endif
#ifdef __cplusplus
extern "C" {
#endif
/* =================================================================
 * TYPE DEFINITIONS - CORE TYPES AND ENUMERATIONS
 * =================================================================
 */

/* Governance Policy Enforcement Pragma */
#pragma rift_policy memory(aligned(4)) type(strict) value(static)

/* Version and Configuration Constants */

/* Maximum number of tokens supported (must fit in uint16_t) */
#ifndef RIFT_TOKENIZER_MAX_TOKENS
#define RIFT_TOKENIZER_MAX_TOKENS 65535U
#endif
#define RIFT_TOKENIZER_VERSION "0.4.0"
#define RIFT_TOKENIZER_MAX_PATTERNS 256
#define RIFT_TOKENIZER_DEFAULT_CAPACITY 1024
#define RIFT_TOKENIZER_BUFFER_SIZE 8192
#define RIFT_TOKENIZER_ERROR_MSG_SIZE 512

/* DFA State Constants */
#define DFA_MAX_STATES 1024
#define DFA_MAX_TRANSITIONS 256
#define DFA_INVALID_STATE_ID UINT32_MAX

/* =================================================================
 * CORE TOKEN STRUCTURES - CANONICAL FROM .BAK
 * =================================================================
 */

/* TokenTriplet - matches .bak specification exactly */
typedef struct __attribute__((packed)) {
    uint32_t type    : 8;   /* Token type identifier (0-255) */
    uint32_t mem_ptr : 16;  /* Memory pointer/offset (0-65535) */
    uint32_t value   : 8;   /* Token value/flags (0-255) */
} TokenTriplet;

/* TokenType - canonical enumeration from .bak */
typedef enum {
    TOKEN_UNKNOWN = 0,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,

    TOKEN_LITERAL_STRING,
    TOKEN_LITERAL_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_DELIMITER,
    TOKEN_R_PATTERN,
    TOKEN_NULL_KEYWORD,
    TOKEN_NIL_KEYWORD,
    TOKEN_WHITESPACE,
    TOKEN_COMMENT,
    TOKEN_EOF,
    /* DFA-specific tokens */
    TOKEN_REGEX_START,
    TOKEN_REGEX_END,
    TOKEN_COMPOSE_AND,
    TOKEN_COMPOSE_OR,
    TOKEN_COMPOSE_XOR,
    TOKEN_COMPOSE_NAND,
    TOKEN_DFA_STATE,
    TOKEN_ERROR = 255
} TokenType;

/* TokenFlags - from .bak with complete flag set */
typedef enum {
    TOKEN_FLAG_NONE       = 0x00,
    TOKEN_FLAG_GLOBAL     = 0x01,  /* g flag */
    TOKEN_FLAG_MULTILINE  = 0x02,  /* m flag */
    TOKEN_FLAG_IGNORECASE = 0x04,  /* i flag */
    TOKEN_FLAG_TOPDOWN    = 0x08,  /* t flag */
    TOKEN_FLAG_BOTTOMUP   = 0x10,  /* b flag */
    TOKEN_FLAG_COMPOSED   = 0x20,  /* Composed regex */
    TOKEN_FLAG_VALIDATED  = 0x40,  /* DFA validated */
    TOKEN_FLAG_ERROR      = 0x80   /* Error state */
} TokenFlags;

/* =================================================================
 * DFA STRUCTURES - CANONICAL FROM .BAK
 * =================================================================
 */

/* DFA State Structure - hierarchical state machine */
typedef struct DFAState {
    uint32_t            state_id;           /* Unique state identifier */
    bool                is_final;           /* Accept state flag */
    TokenType           token_type;         /* Associated token type */
    struct DFAState**   transitions;        /* Transition table [256] */
    size_t              transition_count;   /* Number of transitions */
    uint32_t            flags;              /* State-specific flags */
    void*               user_data;          /* Extensibility pointer */
} DFAState;

/* RegexComposition - DFA-based regex implementation */
typedef struct RegexComposition {
    DFAState*           start_state;        /* Initial DFA state */
    DFAState**          accept_states;      /* Array of accept states */
    size_t              accept_count;       /* Number of accept states */
    TokenFlags          flags;              /* Compilation flags */
    char*               pattern;            /* Original pattern string */
    size_t              pattern_length;    /* Pattern length */
    bool                is_compiled;        /* Compilation complete flag */
} RegexComposition;

/* =================================================================
 * TOKENIZER CONTEXT STRUCTURES - THREAD-SAFE DESIGN
 * =================================================================
 */

/* TokenizerStats - performance monitoring from .bak */
typedef struct TokenizerStats {
    size_t              tokens_processed;   /* Total tokens processed */
    size_t              tokens_generated;   /* Total tokens generated */
    size_t              memory_allocated;   /* Memory currently allocated */
    size_t              memory_peak;        /* Peak memory usage */
    size_t              dfa_states_created; /* Total DFA states */
    size_t              regex_patterns;     /* Compiled regex patterns */
    double              processing_time;    /* Total processing time */
    uint32_t            error_count;        /* Total errors encountered */
} TokenizerStats;

/* TokenizerContext - main tokenizer state container */
typedef struct TokenizerContext {
    /* Core tokenization state */
    TokenTriplet*       tokens;             /* Token array buffer */
    size_t              token_count;        /* Current token count */
    size_t              token_capacity;     /* Buffer capacity */
    
    /* Input processing state */
    const char*         input_buffer;       /* Source text buffer */
    size_t              input_length;       /* Input buffer length */
    size_t              current_position;   /* Current parsing position */
    size_t              line_number;        /* Current line number */
    size_t              column_number;      /* Current column number */
    
    /* DFA and regex state */
    DFAState*           current_dfa_state;  /* Active DFA state */
    RegexComposition**  regex_patterns;     /* Compiled regex array */
    size_t              pattern_count;      /* Number of patterns */
    size_t              pattern_capacity;   /* Pattern array capacity */
    
    /* Thread safety and synchronization */
    pthread_mutex_t     context_mutex;      /* Context protection mutex */
    atomic_bool         thread_safe_mode;   /* Thread safety flag */
    pthread_t           owner_thread;       /* Owning thread ID */
    
    /* Error handling and diagnostics */
    char                error_message[RIFT_TOKENIZER_ERROR_MSG_SIZE];
    uint32_t            error_code;         /* Last error code */
    bool                has_error;          /* Error state flag */
    
    /* Performance statistics */
    TokenizerStats      stats;              /* Runtime statistics */
    
    /* Configuration flags */
    TokenFlags          global_flags;       /* Global tokenizer flags */
    bool                debug_mode;         /* Debug output flag */
    bool                strict_mode;        /* Strict parsing flag */
} TokenizerContext;

/* =================================================================
 * FORWARD DECLARATIONS - BREAKS CIRCULAR DEPENDENCIES
 * =================================================================
 */

/* Function pointer typedefs for extensibility */
typedef TokenTriplet (*TokenCreateFunc)(uint8_t type, uint16_t mem_ptr, uint8_t value);
typedef bool (*TokenValidateFunc)(const TokenTriplet* token);
typedef DFAState* (*DFACreateStateFunc)(uint32_t state_id, bool is_final);
typedef void (*DFADestroyStatesFunc)(DFAState* root);
typedef bool (*DFAAddTransitionFunc)(DFAState* from, DFAState* to, char transition_char);
typedef DFAState* (*DFAProcessInputFunc)(DFAState* start, const char* input, size_t length);
typedef bool (*DFAIsAcceptingStateFunc)(DFAState* state);
typedef TokenType (*DFAGetTokenTypeFunc)(DFAState* state);

/* Error codes for tokenizer operations */
typedef enum {
    RIFT_TOKENIZER_SUCCESS = 0,
    RIFT_TOKENIZER_ERROR_NULL_CONTEXT,
    RIFT_TOKENIZER_ERROR_INVALID_INPUT,
    RIFT_TOKENIZER_ERROR_BUFFER_OVERFLOW,
    RIFT_TOKENIZER_ERROR_DFA_CREATION_FAILED,
    RIFT_TOKENIZER_ERROR_REGEX_COMPILATION_FAILED,
    RIFT_TOKENIZER_ERROR_THREAD_SAFETY_VIOLATION,
    RIFT_TOKENIZER_ERROR_MEMORY_ALLOCATION_FAILED,
    RIFT_TOKENIZER_ERROR_UNKNOWN = 255
} TokenizerErrorCode;

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_TYPES_H */
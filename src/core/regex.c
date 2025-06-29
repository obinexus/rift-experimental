/*
 * rift/src/core/regex.c
 * Simple flag parsing utility for RIFT regex patterns
 */

#include "rift/core/regex.h"

uint32_t rift_regex_parse_flags(const char* flag_string) {
    if (!flag_string) {
        return 0;
    }

    uint32_t flags = 0;
    for (const char* p = flag_string; *p; ++p) {
        switch (*p) {
            case 'g':
                flags |= RIFT_REGEX_GLOBAL;
                break;
            case 'm':
                flags |= RIFT_REGEX_MULTILINE;
                break;
            case 'i':
                flags |= RIFT_REGEX_IGNORECASE;
                break;
            case 't':
                flags |= RIFT_REGEX_TAINTED;
                break;
            default:
                break;
        }
    }
    return flags;
}

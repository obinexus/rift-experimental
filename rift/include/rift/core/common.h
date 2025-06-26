#ifndef RIFT_CORE_COMMON_H
#define RIFT_CORE_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// RIFT Error Codes
#define RIFT_SUCCESS 0
#define RIFT_ERROR_INVALID_ARGUMENT -1
#define RIFT_ERROR_MEMORY_ALLOCATION -2
#define RIFT_ERROR_FILE_ACCESS -3

// RIFT version information
#define RIFT_FRAMEWORK_VERSION_STRING "1.0.0"

const char* rift_get_version_string(void);
const char* rift_get_build_info(void);
const char* rift_error_to_string(int error_code);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_COMMON_H */

/**
 * @file telemetry_governance.c
 * @brief RIFT Governance Telemetry and Violation Logging
 * 
 * Comprehensive telemetry system for tracking governance violations,
 * stage transitions, and memory integrity events.
 * 
 * @copyright Copyright (c) 2025 OBINexus Computing
 * @license Proprietary - All Rights Reserved
 */

#include "memory_governance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

/* Telemetry configuration */
#define TELEMETRY_LOG_FILE "rift_governance_telemetry.log"
#define VIOLATION_LOG_FILE "rift_governance_violations.log"
#define AUDIT_LOG_FILE "rift_governance_audit.log"
#define MAX_LOG_ENTRY_SIZE 1024

/* Log level definitions */
typedef enum {
    TELEMETRY_LEVEL_DEBUG = 0,
    TELEMETRY_LEVEL_INFO = 1,
    TELEMETRY_LEVEL_WARNING = 2,
    TELEMETRY_LEVEL_ERROR = 3,
    TELEMETRY_LEVEL_CRITICAL = 4
} telemetry_log_level_t;

static const char* log_level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"
};

static bool telemetry_initialized = false;
static FILE* telemetry_file = NULL;
static FILE* violation_file = NULL;
static FILE* audit_file = NULL;

/**
 * @brief Initialize telemetry logging system
 */
static bool initialize_telemetry(void) {
    if (telemetry_initialized) return true;
    
    telemetry_file = fopen(TELEMETRY_LOG_FILE, "a");
    violation_file = fopen(VIOLATION_LOG_FILE, "a");
    audit_file = fopen(AUDIT_LOG_FILE, "a");
    
    if (!telemetry_file || !violation_file || !audit_file) {
        printf("[TELEMETRY] Failed to initialize log files\n");
        return false;
    }
    
    telemetry_initialized = true;
    
    // Log initialization
    log_governance_event(TELEMETRY_LEVEL_INFO, 0, 
                        "RIFT Governance Telemetry System Initialized");
    
    return true;
}

/**
 * @brief Get current timestamp string
 */
static void get_timestamp_string(char* buffer, size_t buffer_size) {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

/**
 * @brief Log governance event with specified level
 */
void log_governance_event(telemetry_log_level_t level, uint64_t token_id, const char* format, ...) {
    if (!initialize_telemetry()) return;
    
    char timestamp[32];
    char message[MAX_LOG_ENTRY_SIZE];
    char full_entry[MAX_LOG_ENTRY_SIZE + 128];
    
    get_timestamp_string(timestamp, sizeof(timestamp));
    
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    snprintf(full_entry, sizeof(full_entry), 
             "[%s] [%s] [TOKEN:%lu] %s\n",
             timestamp, log_level_names[level], token_id, message);
    
    // Write to telemetry log
    if (telemetry_file) {
        fprintf(telemetry_file, "%s", full_entry);
        fflush(telemetry_file);
    }
    
    // Also write to console for important events
    if (level >= TELEMETRY_LEVEL_WARNING) {
        printf("%s", full_entry);
    }
}

/**
 * @brief Log governance violation
 */
void log_governance_violation(const rift_governance_violation_t* violation) {
    if (!violation || !initialize_telemetry()) return;
    
    char timestamp[32];
    char violation_entry[MAX_LOG_ENTRY_SIZE];
    
    get_timestamp_string(timestamp, sizeof(timestamp));
    
    snprintf(violation_entry, sizeof(violation_entry),
             "[%s] VIOLATION_ID=%lu TOKEN_ID=%lu TYPE=0x%02X "
             "ATTEMPTED_STAGE=%u CURRENT_STAGE=%u OVERRIDE=%s MESSAGE=\"%s\"\n",
             timestamp, violation->violation_id, violation->token_id,
             violation->violation_type, violation->attempted_stage,
             violation->current_stage,
             violation->governance_override_available ? "YES" : "NO",
             violation->violation_message);
    
    // Write to violation log
    if (violation_file) {
        fprintf(violation_file, "%s", violation_entry);
        fflush(violation_file);
    }
    
    // Log to telemetry as critical event
    log_governance_event(TELEMETRY_LEVEL_CRITICAL, violation->token_id,
                        "GOVERNANCE VIOLATION: %s", violation->violation_message);
    
    printf("[TELEMETRY] Governance violation logged: ID=%lu\n", violation->violation_id);
}

/**
 * @brief Log stage transition event
 */
void log_stage_transition(uint64_t token_id, uint32_t from_stage, uint32_t to_stage, bool success) {
    const char* status = success ? "SUCCESS" : "FAILED";
    
    log_governance_event(success ? TELEMETRY_LEVEL_INFO : TELEMETRY_LEVEL_ERROR,
                        token_id,
                        "STAGE_TRANSITION: %u -> %u [%s]",
                        from_stage, to_stage, status);
    
    // Additional audit logging for stage transitions
    if (audit_file) {
        char timestamp[32];
        get_timestamp_string(timestamp, sizeof(timestamp));
        
        fprintf(audit_file, 
                "[%s] STAGE_TRANSITION TOKEN=%lu FROM=%u TO=%u STATUS=%s\n",
                timestamp, token_id, from_stage, to_stage, status);
        fflush(audit_file);
    }
}

/**
 * @brief Log memory integrity check
 */
void log_memory_integrity_check(uint64_t token_id, bool integrity_valid, 
                               double entropy_value, uint64_t checksum) {
    log_governance_event(integrity_valid ? TELEMETRY_LEVEL_INFO : TELEMETRY_LEVEL_ERROR,
                        token_id,
                        "MEMORY_INTEGRITY: %s ENTROPY=%.6f CHECKSUM=0x%016lx",
                        integrity_valid ? "VALID" : "INVALID",
                        entropy_value, checksum);
}

/**
 * @brief Log anti-reversion attempt
 */
void log_anti_reversion_attempt(uint64_t token_id, uint32_t attempted_stage, 
                               uint32_t current_stage, bool blocked) {
    log_governance_event(blocked ? TELEMETRY_LEVEL_WARNING : TELEMETRY_LEVEL_ERROR,
                        token_id,
                        "ANTI_REVERSION: Attempt to revert %u -> %u [%s]",
                        current_stage, attempted_stage,
                        blocked ? "BLOCKED" : "ALLOWED");
}

/**
 * @brief Log governance policy enforcement
 */
void log_governance_policy_enforcement(uint64_t token_id, const char* policy_name, 
                                     const char* rule_name, bool enforced) {
    log_governance_event(enforced ? TELEMETRY_LEVEL_INFO : TELEMETRY_LEVEL_WARNING,
                        token_id,
                        "POLICY_ENFORCEMENT: %s.%s [%s]",
                        policy_name, rule_name,
                        enforced ? "ENFORCED" : "BYPASSED");
}

/**
 * @brief Log entropy validation
 */
void log_entropy_validation(uint64_t token_id, double calculated_entropy, 
                           double expected_entropy, double tolerance, bool passed) {
    log_governance_event(passed ? TELEMETRY_LEVEL_INFO : TELEMETRY_LEVEL_ERROR,
                        token_id,
                        "ENTROPY_VALIDATION: Calculated=%.6f Expected=%.6f Tolerance=%.6f [%s]",
                        calculated_entropy, expected_entropy, tolerance,
                        passed ? "PASSED" : "FAILED");
}

/**
 * @brief Log context-aware checksum validation
 */
void log_context_checksum_validation(uint64_t token_id, uint64_t calculated_checksum,
                                    uint64_t expected_checksum, bool valid) {
    log_governance_event(valid ? TELEMETRY_LEVEL_INFO : TELEMETRY_LEVEL_ERROR,
                        token_id,
                        "CONTEXT_CHECKSUM: Calculated=0x%016lx Expected=0x%016lx [%s]",
                        calculated_checksum, expected_checksum,
                        valid ? "VALID" : "INVALID");
}

/**
 * @brief Log R"" syntax parsing event
 */
void log_raw_string_parsing(uint64_t token_id, const char* pattern_type, 
                           size_t matches_found, bool parsing_success) {
    log_governance_event(parsing_success ? TELEMETRY_LEVEL_DEBUG : TELEMETRY_LEVEL_WARNING,
                        token_id,
                        "RAW_STRING_PARSING: Type=%s Matches=%zu [%s]",
                        pattern_type, matches_found,
                        parsing_success ? "SUCCESS" : "FAILED");
}

/**
 * @brief Generate telemetry summary report
 */
void generate_telemetry_summary_report(void) {
    if (!initialize_telemetry()) return;
    
    char timestamp[32];
    get_timestamp_string(timestamp, sizeof(timestamp));
    
    printf("\n=== RIFT Governance Telemetry Summary Report ===\n");
    printf("Generated: %s\n", timestamp);
    printf("Log Files:\n");
    printf("  - Telemetry: %s\n", TELEMETRY_LOG_FILE);
    printf("  - Violations: %s\n", VIOLATION_LOG_FILE);
    printf("  - Audit: %s\n", AUDIT_LOG_FILE);
    printf("===============================================\n\n");
    
    // Log the report generation
    log_governance_event(TELEMETRY_LEVEL_INFO, 0,
                        "Telemetry summary report generated");
}

/**
 * @brief Create a governance violation record
 */
rift_governance_violation_t* create_governance_violation(
    uint64_t token_id, uint32_t violation_type, 
    uint32_t attempted_stage, uint32_t current_stage,
    const char* message) {
    
    rift_governance_violation_t* violation = malloc(sizeof(rift_governance_violation_t));
    if (!violation) return NULL;
    
    static uint64_t next_violation_id = 1;
    
    violation->violation_id = next_violation_id++;
    violation->token_id = token_id;
    violation->violation_type = violation_type;
    violation->attempted_stage = attempted_stage;
    violation->current_stage = current_stage;
    violation->timestamp = (uint64_t)time(NULL);
    violation->governance_override_available = false;
    
    strncpy(violation->violation_message, message, sizeof(violation->violation_message) - 1);
    violation->violation_message[sizeof(violation->violation_message) - 1] = '\0';
    
    // Determine if governance override is available
    if (violation_type == RIFT_VIOLATION_ANTI_REVERSION_BLOCKED ||
        violation_type == RIFT_VIOLATION_INVALID_STAGE_TRANSITION) {
        violation->governance_override_available = true;
    }
    
    // Log the violation
    log_governance_violation(violation);
    
    return violation;
}

/**
 * @brief Cleanup telemetry system
 */
void cleanup_telemetry_system(void) {
    if (!telemetry_initialized) return;
    
    log_governance_event(TELEMETRY_LEVEL_INFO, 0,
                        "RIFT Governance Telemetry System Shutdown");
    
    if (telemetry_file) {
        fclose(telemetry_file);
        telemetry_file = NULL;
    }
    
    if (violation_file) {
        fclose(violation_file);
        violation_file = NULL;
    }
    
    if (audit_file) {
        fclose(audit_file);
        audit_file = NULL;
    }
    
    telemetry_initialized = false;
    printf("[TELEMETRY] Governance telemetry system cleaned up\n");
}

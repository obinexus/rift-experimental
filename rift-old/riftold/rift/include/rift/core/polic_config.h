/*
 * PoLiC Configuration - Build-time Security Settings
 * OBINexus Computing
 */

#ifndef RIFT_POLIC_CONFIG_H
#define RIFT_POLIC_CONFIG_H

/* ===== Build Configuration ===== */
#define POLIC_VERSION_MAJOR 2
#define POLIC_VERSION_MINOR 0
#define POLIC_VERSION_PATCH 0

/* ===== Security Configuration ===== */
#define POLIC_MAX_POLICIES 256
#define POLIC_STACK_CANARY_SIZE 8
#define POLIC_VM_HOOK_ENABLED 1
#define POLIC_SANDBOX_DEFAULT 1

/* ===== Integration Flags ===== */
#define RIFT_POLIC_INTEGRATION 1
#define ZERO_TRUST_ENFORCEMENT 1
#define MMD_DEPENDENCY_TRACKING 1

#endif /* RIFT_POLIC_CONFIG_H */

#include "rift/core/common.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define TEST_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s\n", msg); \
        printf("  Assertion failed: %s\n", #cond); \
        printf("  File: %s, Line: %d\n", __FILE__, __LINE__); \
        return false; \
    } \
} while(0)

#define TEST_PASS(msg) do { \
    printf("PASS: %s\n", msg); \
    return true; \
} while(0)

static bool test_alignment_values(void) {
    size_t alignments[] = {8, 16, 64, 4096};
    for (size_t i = 0; i < sizeof(alignments)/sizeof(alignments[0]); ++i) {
        size_t al = alignments[i];
        void* ptr = rift_aligned_alloc(128, al);
        TEST_ASSERT(ptr != NULL, "Allocation failed");
        TEST_ASSERT(((uintptr_t)ptr % al) == 0, "Pointer not aligned");
        rift_aligned_free(ptr);
    }
    TEST_PASS("Aligned allocations meet requirements");
}

static bool test_fallback_aligned_alloc(void) {
    void* ptr = aligned_alloc(32, 128);
    TEST_ASSERT(ptr != NULL, "aligned_alloc failed");
    TEST_ASSERT(((uintptr_t)ptr % 32) == 0, "aligned_alloc pointer misaligned");
#ifdef _POSIX_VERSION
    free(ptr);
#else
    rift_aligned_free(ptr);
#endif
    TEST_PASS("aligned_alloc wrapper works");
}

int main(void) {
    int tests_run = 0, tests_passed = 0;
    printf("Memory Alignment Tests\n");
    if (test_alignment_values()) tests_passed++; tests_run++;
    if (test_fallback_aligned_alloc()) tests_passed++; tests_run++;
    printf("%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}


#include "rift/core/common.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _POSIX_VERSION
#include <errno.h>
#endif

void* rift_aligned_alloc(size_t size, size_t alignment) {
#ifdef _POSIX_VERSION
    void* ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
    return ptr;
#else
    if (alignment < sizeof(void*))
        alignment = sizeof(void*);
    uintptr_t mask = (uintptr_t)alignment - 1;
    void* raw = malloc(size + alignment - 1 + sizeof(void*));
    if (!raw)
        return NULL;
    uintptr_t raw_addr = (uintptr_t)raw + sizeof(void*);
    uintptr_t aligned_addr = (raw_addr + mask) & ~mask;
    ((void**)aligned_addr)[-1] = raw;
    return (void*)aligned_addr;
#endif
}

void rift_aligned_free(void* ptr) {
    if (!ptr)
        return;
#ifdef _POSIX_VERSION
    free(ptr);
#else
    free(((void**)ptr)[-1]);
#endif
}

void rift_memory_block_init(rift_memory_block_t* block, void* ptr,
                            size_t size, size_t alignment,
                            const char* allocator_name) {
    if (!block)
        return;
    block->ptr = ptr;
    block->size = size;
    block->alignment = alignment;
    block->is_aligned = ptr ? (((uintptr_t)ptr % alignment) == 0) : false;
    block->allocator_name = allocator_name;
}

void rift_memory_block_cleanup(rift_memory_block_t* block) {
    if (!block)
        return;
    block->ptr = NULL;
    block->size = 0;
    block->alignment = 0;
    block->is_aligned = false;
    block->allocator_name = NULL;
}

#ifndef aligned_alloc
void* aligned_alloc(size_t alignment, size_t size) {
    return rift_aligned_alloc(size, alignment);
}
#endif


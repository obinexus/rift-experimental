#ifndef RIFT_TOKEN_MEMORY_H
#define RIFT_TOKEN_MEMORY_H

void rift_tokenmemory_destroy(RiftTokenMemory* memory) ;
RiftResult rift_tokenmemory_load_stage0(RiftTokenMemory* memory, const char* rift0_file) ;
RiftResult rift_tokenmemory_validate_tokens(RiftTokenMemory* memory) ;
RiftResult rift_tokenmemory_enhance_tokens(RiftTokenMemory* memory) ;

#endif // RIFT_TOKEN_MEMORY_H

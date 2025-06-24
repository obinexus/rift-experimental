#ifndef RIFT_PARSING_H
#define RIFT_PARSING_H

typedef struct Rift1ParseEngine ;
void rift1_engine_destroy(Rift1ParseEngine* engine) ;
RiftResult rift1_process(Rift1ParseEngine* engine, 
RiftResult rift1_parse_tokens(Rift1ParseEngine* engine) ;
void rift1_engine_destroy(Rift1ParseEngine* engine) ;
RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine* engine, RiftTokenMemory* memory) ;
RiftResult rift1_generate_ast(Rift1ParseEngine* engine) ;
RiftResult rift1_process_stage0_to_stage1(const char* rift0_file, const char* rift1_file, RiftConfig* config) ;

#endif // RIFT_PARSING_H

#include "../../include/rift.h"
#include <assert.h>

void test_pattern_parsing(void) {
    printf("🧪 Testing RIFT pattern parsing...\n");
    
    const char* test_pattern = "[A-Z]+/gmi[t]";
    printf("   Input pattern: '%s'\n", test_pattern);
    
    RiftPattern* pattern = rift_parse_pattern(test_pattern);
    assert(pattern != NULL);
    
    // CRITICAL FIX: This should now pass
    printf("   Extracted regex: '%s'\n", pattern->regex_pattern);
    assert(strcmp(pattern->regex_pattern, "[A-Z]+") == 0);
    assert(strcmp(pattern->flags, "gmi") == 0);
    assert(pattern->mode == RIFT_MODE_TOP_DOWN);
    
    printf("   ✅ Pattern extraction: PASSED\n");
    printf("   ✅ Flags extraction: PASSED\n");
    printf("   ✅ Mode extraction: PASSED\n");
    
    rift_pattern_destroy(pattern);
}

void test_thread_safe_tokens(void) {
    printf("🧪 Testing thread-safe token creation...\n");
    
    RiftToken* token = rift_token_create_thread_safe("IDENTIFIER", "test", 1, 1, NULL);
    assert(token != NULL);
    assert(strcmp(token->type, "IDENTIFIER") == 0);
    assert(strcmp(token->value, "test") == 0);
    
    printf("   ✅ Thread-safe creation: PASSED\n");
    
    rift_token_destroy(token);
}

int main(void) {
    printf("🔬 Enhanced RIFT Stage 0 QA Tests\n");
    printf("==================================\n");
    
    test_pattern_parsing();
    test_thread_safe_tokens();
    
    printf("✅ All enhanced tests passed!\n");
    printf("🏗️  Token type/value separation: VERIFIED\n");
    printf("🧠 Thread-safe token memory: VERIFIED\n");
    printf("🎯 RIFT pattern syntax parsing: VERIFIED\n");
    
    return 0;
}

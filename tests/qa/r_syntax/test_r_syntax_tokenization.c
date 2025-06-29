#include "rift-core/qa/r_syntax_tokenizer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("R-Syntax Tokenization Unit Tests\n");
    
    // Test R"" pattern tokenization
    const char* r_quote_pattern = "R\"(?P<test>[a-z]+)\"";
    rift_r_regex_token_t* token = r_syntax_tokenize(r_quote_pattern, strlen(r_quote_pattern));
    
    assert(token != NULL);
    assert(token->type == R_REGEX_BOTTOM_UP);
    assert(token->flags & BOTTOM_MATCHING);
    assert(r_syntax_validate_token(token));
    
    r_syntax_token_destroy(token);
    
    // Test R'' pattern tokenization
    const char* r_apostrophe_pattern = "R'(?P<block>\\{[^}]*\\})'";
    token = r_syntax_tokenize(r_apostrophe_pattern, strlen(r_apostrophe_pattern));
    
    assert(token != NULL);
    assert(token->type == R_REGEX_TOP_DOWN);
    assert(token->flags & TOP_DOWN_MATCHING);
    assert(r_syntax_validate_token(token));
    
    r_syntax_token_destroy(token);
    
    printf("✓ All R-syntax tokenization tests passed\n");
    return 0;
}

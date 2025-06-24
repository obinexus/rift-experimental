// CMocka-based testing for compiler stages
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "rift_compiler.h"

// Test stage 0 - Tokenizer
static void test_tokenizer_basic(void **state) {
    const char* source = "function main() { return 42; }";
    
    TokenList* tokens = tokenize(source);
    
    assert_non_null(tokens);
    assert_int_equal(tokens->count, 8);
    assert_string_equal(tokens->items[0].value, "function");
    assert_int_equal(tokens->items[0].type, TOKEN_KEYWORD);
    
    free_token_list(tokens);
}

// Test stage 1 - Parser
static void test_parser_function_declaration(void **state) {
    TokenList* tokens = create_test_tokens();
    
    AST* ast = parse(tokens);
    
    assert_non_null(ast);
    assert_int_equal(ast->type, AST_FUNCTION_DECL);
    assert_string_equal(ast->function_decl.name, "main");
    
    free_ast(ast);
}

// Test runner
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_tokenizer_basic),
        cmocka_unit_test(test_parser_function_declaration),
        cmocka_unit_test_setup_teardown(test_semantic_analysis, 
                                       setup_compiler, teardown_compiler),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}

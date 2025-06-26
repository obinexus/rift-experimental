cmake_minimum_required(VERSION 3.20)
project(stage0_tokenization VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(stage0_tokenizer
    src/pocrift_tokenizer.c
    src/uscn_normalizer.c
    src/token_minimizer.c
)

target_include_directories(stage0_tokenizer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Test binary for validation
add_executable(test_stage0
    tests/test_tokenizer.c
    src/uscn_normalizer.c
    src/token_minimizer.c
)

target_include_directories(test_stage0 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()
add_test(NAME test_stage0 COMMAND test_stage0)

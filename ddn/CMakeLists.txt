cmake_minimum_required(VERSION 3.14)
project(ddn LANGUAGES C)

# Library for confusion matrix
add_library(ddn_confusion STATIC
    src/confusion_matrix.c
)

target_include_directories(ddn_confusion PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Test executable
add_executable(test_confusion_matrix
    tests/test_confusion_matrix.c
)

target_link_libraries(test_confusion_matrix ddn_confusion)

enable_testing()
add_test(NAME ConfusionMatrixTest COMMAND test_confusion_matrix)

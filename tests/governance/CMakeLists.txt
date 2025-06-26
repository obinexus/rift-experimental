add_executable(test_sinphase test_sinphase.c)

target_link_libraries(test_sinphase PRIVATE rift-sinphase)

target_include_directories(test_sinphase PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_test(NAME SinphaseTest COMMAND test_sinphase)

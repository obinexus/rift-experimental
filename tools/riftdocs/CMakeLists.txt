cmake_minimum_required(VERSION 3.12)
project(RiftDocs VERSION 1.0.0 LANGUAGES C)

# RIFT Documentation Generation System
# Integrated with build orchestration and zero trust enforcement

set(CMAKE_C_STANDARD 11)

# Dependencies
find_package(PkgConfig REQUIRED)

# riftdocs executable
add_executable(riftdocs
    src/main.c
    src/doc_generator.c
    src/spec_validator.c
    src/audit_binder.c
)

target_include_directories(riftdocs PRIVATE
    include
    ../build_orchestration
    ../../sinphase
)

# Link with hash table and sinphase systems
target_link_libraries(riftdocs
    rift_hash_table
    rift_sinphase
)

# Install riftdocs
install(TARGETS riftdocs
    RUNTIME DESTINATION bin
)

# Generate pkg-config
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/riftdocs.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/riftdocs.pc"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/riftdocs.pc"
    DESTINATION lib/pkgconfig
)

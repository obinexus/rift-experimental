add_library(rift-sinphase STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/governance/sinphase.c
)

target_include_directories(rift-sinphase PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

set_target_properties(rift-sinphase PROPERTIES
    VERSION ${PROJECT_VERSION}
)

install(TARGETS rift-sinphase
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/rift/governance
)

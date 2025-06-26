# FindRIFT.cmake
# CMake module for finding RIFT components
# OBINexus Computing Framework

find_path(RIFT_INCLUDE_DIR
    NAMES rift/rift.h
    PATHS ${CMAKE_SOURCE_DIR}/include
)

find_library(RIFT_CORE_LIBRARY
    NAMES rift_core
    PATHS ${CMAKE_SOURCE_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RIFT
    REQUIRED_VARS RIFT_INCLUDE_DIR
)

if(RIFT_FOUND)
    set(RIFT_INCLUDE_DIRS ${RIFT_INCLUDE_DIR})
    set(RIFT_LIBRARIES ${RIFT_CORE_LIBRARY})
endif()

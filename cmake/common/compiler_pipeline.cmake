# =================================================================
# compiler_pipeline.cmake - RIFT Common Build Pipeline
# RIFT: RIFT Is a Flexible Translator
# Component: Multi-stage build orchestration with governance
# OBINexus Computing Framework - Build Infrastructure
# =================================================================

cmake_minimum_required(VERSION 3.16)

# Set deterministic build policies for reproducible artifacts
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)  # INTERPROCEDURAL_OPTIMIZATION
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)  # option() honors normal variables

# Establish deterministic timestamp for builds
if(NOT DEFINED SOURCE_DATE_EPOCH)
    string(TIMESTAMP SOURCE_DATE_EPOCH "%s" UTC)
endif()

# Global stage directories configuration
set(RIFT_STAGES_MAX 6)
set(RIFT_STAGE_OBJ_ROOT "${CMAKE_BINARY_DIR}/obj")
set(RIFT_STAGE_LIB_ROOT "${CMAKE_BINARY_DIR}/lib")
set(RIFT_STAGE_INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include")
set(RIFT_CONFIG_ROOT "${CMAKE_BINARY_DIR}")

# Compiler flags for deterministic builds with governance
set(RIFT_COMMON_C_FLAGS "-std=c99 -Wall -Wextra -Werror -MMD -fPIC")
set(RIFT_COMMON_C_FLAGS "${RIFT_COMMON_C_FLAGS} -DSOURCE_DATE_EPOCH=${SOURCE_DATE_EPOCH}")

# Thread safety flags for Gosilang integration
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(RIFT_COMMON_C_FLAGS "${RIFT_COMMON_C_FLAGS} -pthread")
endif()

# Stage validation tracking
set(RIFT_VALIDATED_STAGES "" CACHE INTERNAL "List of validated stages")

# =================================================================
# Function: add_rift_stage
# Purpose: Creates stage-specific build targets with governance validation
# Parameters:
#   STAGE_NAME: Stage identifier (e.g., "rift-0", "rift-1")
#   COMPONENT_NAME: Component being built (e.g., "tokenizer", "parser")
# =================================================================
function(add_rift_stage STAGE_NAME COMPONENT_NAME)
    # Extract stage number from STAGE_NAME (e.g., "rift-0" -> "0")
    string(REGEX REPLACE "^rift-([0-9]+)$" "\\1" STAGE_NUM "${STAGE_NAME}")
    
    if(NOT STAGE_NUM MATCHES "^[0-6]$")
        message(FATAL_ERROR "Invalid stage name: ${STAGE_NAME}. Must be rift-0 through rift-6")
    endif()
    
    message(STATUS "Configuring stage ${STAGE_NUM}: ${COMPONENT_NAME}")
    
    # Create stage-specific directories
    set(STAGE_OBJ_DIR "${RIFT_STAGE_OBJ_ROOT}/${STAGE_NAME}")
    set(STAGE_LIB_DIR "${RIFT_STAGE_LIB_ROOT}/${STAGE_NAME}")
    set(STAGE_INCLUDE_DIR "${RIFT_STAGE_INCLUDE_ROOT}/${STAGE_NAME}")
    
    file(MAKE_DIRECTORY "${STAGE_OBJ_DIR}")
    file(MAKE_DIRECTORY "${STAGE_LIB_DIR}")
    
    # Collect source files for this stage
    set(STAGE_SOURCES "")
    
    # Core sources
    file(GLOB_RECURSE CORE_SOURCES 
         "${CMAKE_SOURCE_DIR}/src/core/*.c")
    list(APPEND STAGE_SOURCES ${CORE_SOURCES})
    
    # CLI sources (if they exist)
    file(GLOB_RECURSE CLI_SOURCES 
         "${CMAKE_SOURCE_DIR}/src/cli/*.c")
    list(APPEND STAGE_SOURCES ${CLI_SOURCES})
    
    # Governance sources
    file(GLOB_RECURSE GOV_SOURCES 
         "${CMAKE_SOURCE_DIR}/src/gov/*.c")
    list(APPEND STAGE_SOURCES ${GOV_SOURCES})
    
    if(NOT STAGE_SOURCES)
        message(WARNING "No sources found for stage ${STAGE_NAME}")
        return()
    endif()
    
    # Create object library for stage
    set(OBJECTS_TARGET "${STAGE_NAME}_objects")
    add_library(${OBJECTS_TARGET} OBJECT ${STAGE_SOURCES})
    
    # Configure object library properties
    target_compile_options(${OBJECTS_TARGET} PRIVATE ${RIFT_COMMON_C_FLAGS})
    
    # Set output directory for object files
    set_target_properties(${OBJECTS_TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${STAGE_OBJ_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${STAGE_OBJ_DIR}"
    )
    
    # Configure include directories
    target_include_directories(${OBJECTS_TARGET} PRIVATE
        "${STAGE_INCLUDE_DIR}/core"
        "${STAGE_INCLUDE_DIR}/cli"
        "${CMAKE_SOURCE_DIR}/include/${STAGE_NAME}/core"
        "${CMAKE_SOURCE_DIR}/include/${STAGE_NAME}/cli"
    )
    
    # Create static library from objects
    set(STATIC_TARGET "${STAGE_NAME}_static")
    add_library(${STATIC_TARGET} STATIC $<TARGET_OBJECTS:${OBJECTS_TARGET}>)
    
    # Configure static library properties
    set_target_properties(${STATIC_TARGET} PROPERTIES
        OUTPUT_NAME "${STAGE_NAME}"
        ARCHIVE_OUTPUT_DIRECTORY "${STAGE_LIB_DIR}"
        PREFIX "lib"
        SUFFIX ".a"
    )
    
    # Create shared library from objects (optional)
    if(BUILD_SHARED_LIBS)
        set(SHARED_TARGET "${STAGE_NAME}_shared")
        add_library(${SHARED_TARGET} SHARED $<TARGET_OBJECTS:${OBJECTS_TARGET}>)
        
        set_target_properties(${SHARED_TARGET} PROPERTIES
            OUTPUT_NAME "${STAGE_NAME}"
            LIBRARY_OUTPUT_DIRECTORY "${STAGE_LIB_DIR}"
            PREFIX "lib"
            SUFFIX ".so"
        )
    endif()
    
    # Generate dependency files for incremental builds
    set(DEP_FILE "${STAGE_OBJ_DIR}/stage_${STAGE_NUM}.d")
    set(DEP_TARGET "${STAGE_NAME}_depfile")
    add_custom_target(${DEP_TARGET}
        COMMAND ${CMAKE_COMMAND} -E touch "${DEP_FILE}"
        DEPENDS ${OBJECTS_TARGET}
        COMMENT "Generating dependency file for stage ${STAGE_NUM}"
    )

    add_dependencies(${STATIC_TARGET} ${DEP_TARGET})
    if(BUILD_SHARED_LIBS)
        add_dependencies(${SHARED_TARGET} ${DEP_TARGET})
    endif()
    
    # Configure stage-specific .riftrc file
    configure_riftrc_stage(${STAGE_NUM} ${COMPONENT_NAME})
    
    message(STATUS "Stage ${STAGE_NUM} configuration complete: ${STAGE_NAME}")
endfunction()

# =================================================================
# Function: configure_riftrc_stage
# Purpose: Generate stage-specific .riftrc configuration files
# Parameters:
#   STAGE_NUM: Stage number (0-6)
#   COMPONENT_NAME: Component being configured
# =================================================================
function(configure_riftrc_stage STAGE_NUM COMPONENT_NAME)
    set(RIFTRC_FILE "${RIFT_CONFIG_ROOT}/.riftrc.${STAGE_NUM}")
    
    # Generate stage-specific configuration
    file(WRITE "${RIFTRC_FILE}"
"# RIFT Stage ${STAGE_NUM} Configuration
# Component: ${COMPONENT_NAME}
# Generated: ${SOURCE_DATE_EPOCH}

[stage]
number = ${STAGE_NUM}
component = \"${COMPONENT_NAME}\"
max_stages = ${RIFT_STAGES_MAX}

[memory]
# Token type/value separation for SSA compliance
enforce_separation = true
bitfield_validation = true
thread_safe_encoding = true

[tokenizer]
# DFA-based tokenization settings
dfa_enabled = true
r_pattern_support = true
null_nil_semantics = \"context_sensitive\"

[compilation]
# Deterministic build settings
timestamp = ${SOURCE_DATE_EPOCH}
reproducible = true
dependency_tracking = true

[governance]
# Stage progression validation
entry_validation = true
exit_validation = true
type_safety_check = true
architecture_compliance = true

[paths]
obj_dir = \"obj/rift-${STAGE_NUM}\"
lib_dir = \"lib/rift-${STAGE_NUM}\"
include_dir = \"include/rift-${STAGE_NUM}\"
")
    
    message(STATUS "Generated .riftrc.${STAGE_NUM} for ${COMPONENT_NAME}")
endfunction()

# =================================================================
# Function: add_rift_validation
# Purpose: Add validation target for stage progression
# Parameters:
#   STAGE_NAME: Stage to validate
# =================================================================
function(add_rift_validation STAGE_NAME)
    string(REGEX REPLACE "^rift-([0-9]+)$" "\\1" STAGE_NUM "${STAGE_NAME}")
    
    set(VALIDATION_TARGET "validate_${STAGE_NAME}")
    set(RIFTRC_FILE "${RIFT_CONFIG_ROOT}/.riftrc.${STAGE_NUM}")
    
    add_custom_target(${VALIDATION_TARGET}
        COMMAND ${CMAKE_COMMAND} -E echo "Validating stage ${STAGE_NUM}..."
        COMMAND test -f "${RIFTRC_FILE}" || (echo "Missing .riftrc.${STAGE_NUM}" && exit 1)
        COMMAND ${CMAKE_COMMAND} -E echo "Stage ${STAGE_NUM} validation complete"
        DEPENDS ${STAGE_NAME}_static
        COMMENT "Validating RIFT stage ${STAGE_NUM} compliance"
    )
    
    # Add to validated stages list
    list(APPEND RIFT_VALIDATED_STAGES ${STAGE_NUM})
    set(RIFT_VALIDATED_STAGES "${RIFT_VALIDATED_STAGES}" CACHE INTERNAL "List of validated stages")
    
    message(STATUS "Added validation target for stage ${STAGE_NUM}")
endfunction()

# =================================================================
# Function: generate_governance_config
# Purpose: Create master governance configuration
# =================================================================
function(generate_governance_config)
    set(GOV_RIFTRC_FILE "${RIFT_CONFIG_ROOT}/gov.riftrc")
    
    file(WRITE "${GOV_RIFTRC_FILE}"
"# RIFT Governance Configuration
# Master policy manifest (read-only)
# Applies to all stages (0-6)

[governance]
version = \"1.0\"
policy_as_code = true
hierarchical_rules = true
audit_trail = true

[compilation]
# Global compilation policies
deterministic_builds = true
stage_isolation = true
dependency_validation = true
cross_contamination_prevention = true

[memory_safety]
# Memory management policies
bounds_checking = true
use_after_free_detection = true
static_analysis = true
runtime_monitoring = true

[thread_safety]
# Parallelism policies
parent_child_spawning = true
read_only_configs = true
mutex_patterns = true
gosilang_integration = true

[validation]
# Stage progression policies
progressive_gates = true
entry_exit_criteria = true
schema_validation = true
type_value_separation = true

[toolchain]
# Build toolchain policies
cmake_version_min = \"3.16\"
c_standard = \"c99\"
compiler_flags = \"${RIFT_COMMON_C_FLAGS}\"

[artifacts]
# Build artifact policies
normalized_input = true
timestamp_standardization = true
content_hashing = true
isolated_environments = true
")
    
    message(STATUS "Generated governance configuration: gov.riftrc")
endfunction()

# Initialize governance on first configure
if(NOT EXISTS "${RIFT_CONFIG_ROOT}/gov.riftrc")
    generate_governance_config()
endif()

message(STATUS "RIFT compiler pipeline configuration loaded")
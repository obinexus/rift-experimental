# RIFT Stage 0 Edge Case Registry

## Pattern Parsing Edge Cases

### 1. Malformed RIFT Patterns
- **Case**: `R"[unclosed` (missing closing quote)
- **Expected**: Return NULL, log error
- **Validation**: Pattern parser should reject malformed syntax
- **Status**: ✅ Implemented with NULL checks

### 2. Empty Patterns  
- **Case**: `R""/g[t]"` (empty regex)
- **Expected**: Valid pattern object with empty regex
- **Validation**: Should compile but match nothing
- **Status**: ✅ Implemented with proper memory handling

### 3. Invalid Modes
- **Case**: `R"test/g[x]"` (invalid mode character)
- **Expected**: Default to top-down mode ('t')
- **Validation**: Mode should fallback gracefully
- **Status**: ✅ Implemented with default fallback

## Memory Management Edge Cases

### 4. NULL Pointer Handling
- **Case**: All functions called with NULL parameters
- **Expected**: Graceful failure, no crashes
- **Validation**: Defensive programming throughout
- **Status**: ✅ Comprehensive NULL checks implemented

### 5. Memory Allocation Failures
- **Case**: malloc/calloc returns NULL
- **Expected**: Proper cleanup and error reporting
- **Validation**: All allocations checked
- **Status**: ✅ Error handling with cleanup

### 6. String Duplication Failures
- **Case**: strdup returns NULL (low memory)
- **Expected**: Cascade cleanup of partial objects
- **Validation**: Pattern/token creation should fail safely
- **Status**: ✅ Proper error propagation

## Compiler Warning Elimination

### 7. Unused Variables
- **Case**: Variables declared but not used
- **Expected**: No compiler warnings with -Werror -Wall -Wextra
- **Validation**: All variables must be used or marked
- **Status**: ✅ Fixed with proper variable usage

### 8. Implicit Function Declarations
- **Case**: Missing #include or function declarations
- **Expected**: Clean compilation with strict flags
- **Validation**: All functions properly declared
- **Status**: ✅ Added proper headers and defines

### 9. Type Conversion Warnings
- **Case**: Assignment between incompatible types
- **Expected**: Explicit casting or proper types
- **Validation**: No implicit conversions
- **Status**: ✅ Fixed with proper error handling

## CLI and Configuration Edge Cases

### 10. Missing Configuration File
- **Case**: .riftrc not found
- **Expected**: Use defaults, continue processing
- **Validation**: Optional configuration loading
- **Status**: ✅ Graceful degradation

### 11. Invalid Command Line Arguments
- **Case**: Unknown flags or missing required args
- **Expected**: Usage message and exit
- **Validation**: Proper argument validation
- **Status**: ✅ getopt_long with validation

### 12. Output Directory Creation
- **Case**: Cannot create output directory
- **Expected**: Warning but continue if possible
- **Validation**: mkdir command execution
- **Status**: ✅ System call with error checking

## IR Generation Edge Cases

### 13. Large Token Count
- **Case**: Input generates > 1000 tokens
- **Expected**: Safety limit to prevent memory exhaustion
- **Validation**: Token counting with limits
- **Status**: ✅ Implemented with 1000 token limit

### 14. Invalid Output File Path
- **Case**: Cannot write to output location
- **Expected**: Clear error message and cleanup
- **Validation**: File I/O error handling
- **Status**: ✅ Proper error reporting

### 15. Empty Input Files
- **Case**: Zero-length input file
- **Expected**: Valid .rift.0 output with zero tokens
- **Validation**: Empty file processing
- **Status**: ✅ Handled gracefully

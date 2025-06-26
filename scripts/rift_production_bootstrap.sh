#!/bin/bash
# RIFT Production Bootstrap - Syntax Error Diagnostic & Fix Protocol
# OBINexus Computing - Professional Software Engineering
# Technical Team + Nnamdi Okpala Collaborative Development

set -euo pipefail

# ===== SYNTAX DIAGNOSTIC FRAMEWORK =====
readonly TARGET_SCRIPT="scripts/rift_production_bootstrap.sh"
readonly ERROR_LINE=1137
readonly BACKUP_SUFFIX=".syntax_backup_$(date +%Y%m%d_%H%M%S)"

echo "🔧 RIFT Production Bootstrap Syntax Error Analysis"
echo "=================================================="
echo "Target: $TARGET_SCRIPT"
echo "Error Line: $ERROR_LINE"

# ===== Phase 1: Structural Analysis =====
analyze_script_structure() {
    echo ""
    echo "📊 Phase 1: Structural Analysis"
    echo "==============================="
    
    if [[ ! -f "$TARGET_SCRIPT" ]]; then
        echo "❌ Target script not found: $TARGET_SCRIPT"
        return 1
    fi
    
    # Create backup before analysis
    cp "$TARGET_SCRIPT" "${TARGET_SCRIPT}${BACKUP_SUFFIX}"
    echo "✅ Backup created: ${TARGET_SCRIPT}${BACKUP_SUFFIX}"
    
    # Line-by-line syntax validation
    echo "🔍 Running bash syntax check..."
    if bash -n "$TARGET_SCRIPT" 2>&1 | grep -E "line [0-9]+:"; then
        echo "❌ Syntax validation failed - errors detected"
    else
        echo "✅ Syntax validation passed - no bash parser errors"
        return 0
    fi
}

# ===== Phase 2: Control Flow Mapping =====
map_control_structures() {
    echo ""
    echo "🗺️  Phase 2: Control Flow Structure Mapping"
    echo "==========================================="
    
    echo "📋 Analyzing control flow patterns around line $ERROR_LINE..."
    
    # Extract context around error line
    local start_line=$((ERROR_LINE - 20))
    local end_line=$((ERROR_LINE + 10))
    
    echo "Context Lines ${start_line}-${end_line}:"
    echo "----------------------------------------"
    sed -n "${start_line},${end_line}p" "$TARGET_SCRIPT" | nl -v$start_line
    
    echo ""
    echo "🔍 Control structure analysis:"
    
    # Count control structures in problematic region
    local for_count=$(sed -n "${start_line},${end_line}p" "$TARGET_SCRIPT" | grep -c "^[[:space:]]*for " || true)
    local while_count=$(sed -n "${start_line},${end_line}p" "$TARGET_SCRIPT" | grep -c "^[[:space:]]*while " || true)
    local if_count=$(sed -n "${start_line},${end_line}p" "$TARGET_SCRIPT" | grep -c "^[[:space:]]*if " || true)
    local done_count=$(sed -n "${start_line},${end_line}p" "$TARGET_SCRIPT" | grep -c "^[[:space:]]*done" || true)
    local fi_count=$(sed -n "${start_line},${end_line}p" "$TARGET_SCRIPT" | grep -c "^[[:space:]]*fi" || true)
    
    echo "   for loops: $for_count"
    echo "   while loops: $while_count"
    echo "   if statements: $if_count"
    echo "   done statements: $done_count"
    echo "   fi statements: $fi_count"
    
    # Calculate balance
    local loop_balance=$((for_count + while_count - done_count))
    local conditional_balance=$((if_count - fi_count))
    
    echo ""
    echo "📊 Structure Balance Analysis:"
    echo "   Loop balance (for+while-done): $loop_balance"
    echo "   Conditional balance (if-fi): $conditional_balance"
    
    if [[ $loop_balance -lt 0 ]]; then
        echo "❌ ISSUE: Excess 'done' statements detected"
        echo "   Recommendation: Remove $((-loop_balance)) 'done' statement(s)"
    elif [[ $loop_balance -gt 0 ]]; then
        echo "❌ ISSUE: Missing 'done' statements detected"
        echo "   Recommendation: Add $loop_balance 'done' statement(s)"
    else
        echo "✅ Loop structures appear balanced"
    fi
}

# ===== Phase 3: Targeted Line Analysis =====
analyze_specific_lines() {
    echo ""
    echo "🎯 Phase 3: Targeted Line Analysis"
    echo "=================================="
    
    # Extract the exact problematic line and surrounding context
    echo "Examining lines 1135-1140:"
    sed -n '1135,1140p' "$TARGET_SCRIPT" | nl -v1135
    
    echo ""
    echo "🔍 Pattern matching for common syntax errors:"
    
    # Check for common bash syntax issues
    local line_1137=$(sed -n '1137p' "$TARGET_SCRIPT")
    echo "Line 1137 content: '$line_1137'"
    
    # Analyze the specific line
    if [[ "$line_1137" =~ ^[[:space:]]*done[[:space:]]*$ ]]; then
        echo "❌ Line 1137 is a standalone 'done' statement"
        echo "🔍 Searching for unmatched loop structures above..."
        
        # Look backwards for unmatched loops
        sed -n '1,1136p' "$TARGET_SCRIPT" | tac | nl | head -50
    else
        echo "ℹ️  Line 1137 is not a simple 'done' statement"
        echo "🔍 Content analysis required for complex syntax error"
    fi
}

# ===== Phase 4: Automated Fix Generation =====
generate_syntax_fixes() {
    echo ""
    echo "🛠️  Phase 4: Automated Fix Generation"
    echo "===================================="
    
    # Create a corrected version
    local fixed_script="${TARGET_SCRIPT}.fixed"
    
    echo "📝 Generating potential fixes..."
    
    # Strategy 1: Remove excess 'done' statements
    echo "🔧 Fix Strategy 1: Remove excess 'done' at line 1137"
    sed '1137d' "$TARGET_SCRIPT" > "${fixed_script}_strategy1"
    
    # Strategy 2: Add missing loop opening
    echo "🔧 Fix Strategy 2: Add missing loop structure"
    sed '1136a\for item in "${items[@]}"; do' "$TARGET_SCRIPT" > "${fixed_script}_strategy2"
    
    # Strategy 3: Fix malformed loop ending
    echo "🔧 Fix Strategy 3: Fix malformed control structure"
    sed '1137s/done/fi/' "$TARGET_SCRIPT" > "${fixed_script}_strategy3"
    
    echo ""
    echo "🧪 Testing generated fixes:"
    
    for strategy in strategy1 strategy2 strategy3; do
        echo "   Testing ${strategy}..."
        if bash -n "${fixed_script}_${strategy}" 2>/dev/null; then
            echo "   ✅ ${strategy}: Syntax validation passed"
        else
            echo "   ❌ ${strategy}: Syntax validation failed"
        fi
    done
}

# ===== Phase 5: Manual Fix Recommendations =====
provide_manual_fixes() {
    echo ""
    echo "📋 Phase 5: Manual Fix Recommendations"
    echo "======================================"
    
    echo "Based on analysis, here are the recommended approaches:"
    echo ""
    echo "Option A: Line Deletion Method"
    echo "   sed -i '1137d' $TARGET_SCRIPT"
    echo "   # Removes the problematic 'done' statement"
    echo ""
    echo "Option B: Context-Aware Debugging"
    echo "   1. Examine lines 1100-1140 for loop/conditional structure"
    echo "   2. Identify the unmatched construct"
    echo "   3. Either add missing opening or remove excess closing"
    echo ""
    echo "Option C: Block Comment Approach"
    echo "   sed -i '1137s/^/# DISABLED: /' $TARGET_SCRIPT"
    echo "   # Comments out the problematic line for testing"
    echo ""
    echo "🎯 Recommended Immediate Action:"
    echo "   1. Review the control flow mapping output above"
    echo "   2. Apply Option A if excess 'done' confirmed"
    echo "   3. Test with: bash -n $TARGET_SCRIPT"
    echo "   4. Execute with: ./$TARGET_SCRIPT --dry-run --validate"
}

# ===== Execution Framework =====
main() {
    echo "🏗️  AEGIS Methodology: Systematic Syntax Error Resolution"
    echo "Technical Lead: Collaborative Development (Technical Team + Nnamdi Okpala)"
    echo ""
    
    analyze_script_structure
    map_control_structures  
    analyze_specific_lines
    generate_syntax_fixes
    provide_manual_fixes
    
    echo ""
    echo "✅ Syntax diagnostic protocol completed"
    echo "📁 Backup preserved: ${TARGET_SCRIPT}${BACKUP_SUFFIX}"
    echo "🎯 Next: Apply recommended fix and validate with bash -n"
}

main "$@"
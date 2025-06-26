#!/bin/bash
# RIFT Incremental Edit Helper - Manage document sections via stage artifacts

set -euo pipefail

# Discover latest stage directory like rift-0, rift-1, ...
latest=$(ls -d rift-[0-9]* 2>/dev/null | sed -E 's/.*rift-([0-9]+)$/\1/' | sort -n | tail -1)
if [[ -z "$latest" ]]; then
    echo "No RIFT stage directories found" >&2
    exit 1
fi

alias rift-exe="rift-${latest}.exe"
echo "Using stage rift-${latest}"

while true; do
    read -rp "Edit prompt: " edit_input || break
    next=$((latest + 1))
    cp "current.riftrc.${latest}.in" "current.riftrc.${next}.in" 2>/dev/null || true
    # apply_edit is a user-provided function that edits the input file
    apply_edit "current.riftrc.${next}.in" "$edit_input"
    "rift-${next}.exe" "current.riftrc.${next}.in" -o build
    latest=$next
    alias rift-exe="rift-${latest}.exe"
    echo "Bumped to stage rift-${latest}"
done

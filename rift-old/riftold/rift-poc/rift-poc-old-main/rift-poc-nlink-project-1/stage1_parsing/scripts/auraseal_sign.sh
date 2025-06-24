#!/bin/bash
echo "Applying AuraSeal signature to: $1"
echo "# AuraSeal: STAGE_1_PARSED_$(date +%s)" >> "$1"
echo "✓ AuraSeal applied"

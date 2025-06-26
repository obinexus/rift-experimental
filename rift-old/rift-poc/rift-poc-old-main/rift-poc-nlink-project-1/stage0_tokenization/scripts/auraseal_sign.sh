#!/bin/bash
echo "Applying AuraSeal signature to: $1"
echo "# AuraSeal: STAGE_0_TOKENIZED_$(date +%s)" >> "$1"
echo "✓ AuraSeal applied"

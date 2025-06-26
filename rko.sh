#!/bin/bash
# rko.sh - Rebase Known Object (RKO) Submodel into Monorepo

# Usage: ./rko.sh <target-dir>
# Example: ./rko.sh riftgossip

set -e

# Require argument
if [ -z "$1" ]; then
  echo "Usage: $0 <submodel-folder>"
  exit 1
fi

TARGET_DIR="$1"
NEW_BRANCH="import-$TARGET_DIR"
FINAL_DIR="rift-old/$TARGET_DIR"

echo "[rko] 🔍 Target directory: $TARGET_DIR"

# 1. Kill internal .git if exists
if [ -d "$TARGET_DIR/.git" ]; then
  echo "[rko] 🚮 Removing .git from $TARGET_DIR"
  rm -rf "$TARGET_DIR/.git"
fi

# 2. Move into monorepo
if [ ! -d "$FINAL_DIR" ]; then
  echo "[rko] 📦 Moving $TARGET_DIR into $FINAL_DIR"
  mkdir -p "$(dirname "$FINAL_DIR")"
  mv "$TARGET_DIR" "$FINAL_DIR"
fi

# 3. Git add and commit
git checkout -b "$NEW_BRANCH"
git add "$FINAL_DIR"
git commit -m "chore: import $TARGET_DIR into rift-old structure"

# 4. Hint to finish rebase/merge
echo "[rko] ✅ Done. To finalize:"
echo "  git checkout main"
echo "  git merge $NEW_BRANCH"


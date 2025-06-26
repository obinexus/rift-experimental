#!/usr/bin/env bash
# Basic policy validation for RIFT governance
# Checks for required .riftrc.N files

set -euo pipefail

usage() {
    echo "Usage: $0 --stage=<N|all>" >&2
    exit 2
}

dir="$(cd "$(dirname "$0")" && pwd)"
stage_arg="all"

for arg in "$@"; do
    case "$arg" in
        --stage=*) stage_arg="${arg#*=}" ;;
        -*) usage ;;
    esac
    shift || true
done

check_stage() {
    local s="$1"
    local f="$dir/.riftrc.$s"
    if [[ -f "$f" ]]; then
        echo "Found $f"
    else
        echo "Missing $f" >&2
        return 1
    fi
}

if [[ "$stage_arg" == "all" ]]; then
    result=0
    for i in {0..6}; do
        if ! check_stage "$i"; then
            result=1
        fi
    done
    exit $result
elif [[ "$stage_arg" =~ ^[0-6]$ ]]; then
    check_stage "$stage_arg"
    exit $?
else
    usage
fi

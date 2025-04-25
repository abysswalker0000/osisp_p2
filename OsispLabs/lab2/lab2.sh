#!/bin/bash

INPUT="input.txt"
OUTPUT="output.txt"

if [[ ! -f "$INPUT" ]]; then
    printf "Error: Input file '%s' does not exist.\n" "$INPUT" >&2
    exit 1
fi

sed -E ':a;N;$!ba; s/^[[:space:]]*[a-z]/\U&/; s/([.!?])[[:space:]]+([a-z])/\1 \U\2/g' "$INPUT" > "$OUTPUT"


if [[ $? -ne 0 ]]; then
    printf "Error: Failed to process the file with sed.\n" >&2
    exit 1
fi
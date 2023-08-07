#!/bin/bash

SOURCE_FILES=$(find "Source" -type f \( -name "*.cpp" -o -name "*.h" \))

echo "Checking clang-format for files:"
echo "$SOURCE_FILES"

clang-format --dry-run -style=file $SOURCE_FILES -Werror

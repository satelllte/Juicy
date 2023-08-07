#!/bin/bash

clang-format --version
echo "---"

SOURCE_FILES=$(find "Source" -type f \( -name "*.cpp" -o -name "*.h" \))

echo "Checking formatting for files:"
echo "$SOURCE_FILES"

clang-format --dry-run -style=file $SOURCE_FILES -Werror

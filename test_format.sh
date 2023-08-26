#!/bin/bash

clang-format --version
echo "---"

SOURCE_FILES=$(find "source" -type f \( -name "*.cpp" -o -name "*.h" \))

echo "Checking formatting for files:"
echo "$SOURCE_FILES"

clang-format --dry-run -style=file $SOURCE_FILES -Werror

#!/bin/bash

clang-format --version
echo "---"

SOURCE_FILES=$(find "source" -type f \( -name "*.cpp" -o -name "*.h" \))

echo "Formatting files:"
echo "$SOURCE_FILES"

clang-format -i -style=file $SOURCE_FILES -Werror

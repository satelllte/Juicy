#!/bin/bash

clang-tidy --version
echo "---"

SOURCE_FILES=$(find "Source" -type f \( -name "*.cpp" -o -name "*.h" \))

echo "Running linter test for files:"
echo "$SOURCE_FILES"

clang-tidy $SOURCE_FILES -p build

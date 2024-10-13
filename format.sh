#!/bin/bash

find ./src -name "*.cc" | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file" --style=file:./.clang_format
done

find ./src -name "*.h" | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file" --style=file:./.clang_format
done

echo "All files formatted!"

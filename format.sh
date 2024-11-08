#!/bin/bash
echo "WARNING: This script will format all .cpp, .h, .c, .hpp files in the include and src directory."
echo "This script will overwrite the files with the formatted version."

read -p "Do you want to continue? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
fi

if [ ! -f .clang-format ]; then
    echo "ERROR: .clang-format file not found."
    exit 1
fi

if ! command -v clang-format &>/dev/null; then
    echo "ERROR: clang-format not found."
    exit 1
fi

echo "Formatting files..."

find includes src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

echo "Done."

exit 0

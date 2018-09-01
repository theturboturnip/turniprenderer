#!/bin/bash
cd "$(dirname $0)"

header_files=$(find -name "*.h" -o -name "*.hpp") # Doesn't produce a list, produces a string
c_files=$(find -name "*.c" -o -name "*.cpp")
printf "set(TR_DEMO_HEADER_FILES $header_files)\nset(TR_DEMO_SRC_FILES $c_files)" > CMakeSources.txt

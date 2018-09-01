#!/bin/bash
cd "$(dirname $0)"

PUBLIC_INCLUDE_DIR=include/
PRIVATE_INCLUDE_DIR=include/turniprenderer/

header_files=$(find src/ ${PRIVATE_INCLUDE_DIR} -name "*.h" -o -name "*.hpp") # Doesn't produce a list, produces a string
c_files=$(find src/ -name "*.c" -o -name "*.cpp")
printf "set(TR_PUBLIC_INCLUDE_DIR \${CMAKE_CURRENT_SOURCE_DIR}/$PUBLIC_INCLUDE_DIR CACHE STRING \"Include Directory for TurnipRenderer\")\nset(TR_PRIVATE_INCLUDE_DIR \${CMAKE_CURRENT_SOURCE_DIR}/$PRIVATE_INCLUDE_DIR)\nset(TR_HEADER_FILES $header_files)\nset(TR_SRC_FILES $c_files)" > CMakeSources.txt

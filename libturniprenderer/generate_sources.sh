#!/bin/bash
cd "$(dirname $0)"

INCLUDE_DIR=include/turniprenderer/

header_files=$(find src/ ${INCLUDE_DIR} -name "*.h" -o -name "*.hpp") # Doesn't produce a list, produces a string
c_files=$(find src/ -name "*.c" -o -name "*.cpp")
printf "set(TR_INCLUDE_DIR $INCLUDE_DIR)\nset(TR_HEADER_FILES $header_files)\nset(TR_SRC_FILES $c_files)" > CMakeSources.txt
#printf "add_library(turniprenderer STATIC $header_files $c_files \${EXTERNAL_SRC_FILES})\n\ntarget_include_directories(turniprenderer PRIVATE ${INCLUDE_DIR} \${EXTERNAL_INCLUDE_DIRS})\n" > CMakeSources.txt

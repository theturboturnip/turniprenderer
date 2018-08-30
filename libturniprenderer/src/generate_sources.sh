#!/bin/bash
cd "$(dirname $0)"
INCLUDE_DIR=../include/turniprenderer
header_files=$(find $INCLUDE_DIR -name *.h)
cpp_files=$(find -name *.cpp)
printf "include_directories($INCLUDE_DIR)\nadd_library(turniprenderer STATIC $header_files $cpp_files)\n" > CMakeLists.txt

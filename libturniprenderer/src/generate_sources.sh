#!/bin/bash
cd "$(dirname $0)"
find_output=$(find -name *.cpp)
echo "add_library(libturnipripper STATIC $find_output)" > CMakeLists.txt

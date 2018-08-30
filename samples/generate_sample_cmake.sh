#!/bin/bash
if [[ $# -eq 0 ]] ; then
    echo "Usage: $0 <sample_name>"
    exit 1
fi
cd "$(dirname $0)/$1"
cpp_files=$(find -name *.cpp)
printf "cmake_minimum_required(VERSION 3.9)\nadd_subdirectory(../../libturniprenderer)\ninclude_directories(../../libturnipripper/include/)\nadd_executable(main $cpp_files)\ntarget_link_libraries(main PRIVATE turniprenderer)" > CMakeLists.txt

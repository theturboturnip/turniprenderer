cmake_minimum_required(VERSION 3.10)

file(GLOB EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/*/include/)
find_package(SDL2 REQUIRED)
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

file(GLOB EXTERNAL_SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/*/src/)

foreach(EXTERNAL_SRC_DIR IN LISTS EXTERNAL_SRC_DIRS)
  message("${EXTERNAL_SRC_DIR}")
  file(GLOB_RECURSE EXTERNAL_SRC_FILES ${EXTERNAL_SRC_DIR}/*.cpp)
endforeach(EXTERNAL_SRC_DIR)

add_subdirectory(libturniprenderer)
add_subdirectory(demo)

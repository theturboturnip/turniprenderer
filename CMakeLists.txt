cmake_minimum_required(VERSION 3.10)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

#set(EXTERNAL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/)
#file(GLOB EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/*/include/)
#list(REMOVE_ITEM EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/include/)

#file(GLOB EXTERNAL_SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/*/src/)
#list(REMOVE_ITEM EXTERNAL_SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/rc/)
#set(EXTERNAL_SRC_DIRS ${EXTERNAL_INCLUDE_DIRS} ${EXTERNAL_SRC_DIRS})

#foreach(EXTERNAL_SRC_DIR IN LISTS EXTERNAL_SRC_DIRS)
#  message("${EXTERNAL_SRC_DIR}")
#  file(GLOB_RECURSE EXTERNAL_SRC_FILES ${EXTERNAL_SRC_DIR}/*.cpp)
#endforeach(EXTERNAL_SRC_DIR)

add_definitions("-O3")

add_subdirectory(external)
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

add_subdirectory(libturniprenderer)
add_subdirectory(demo)

project(libturniprenderer VERSION 1.0.0 DESCRIPTION "Renderer/Game Engine")
  
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeSources.txt)


add_library(turniprenderer-ext STATIC ${EXTERNAL_SRC_FILES})
target_include_directories(turniprenderer-ext PUBLIC ${EXTERNAL_INCLUDE_DIRS})
target_compile_options(turniprenderer-ext PRIVATE "-std=c++1z" "-Wno-everything")

# TODO: This is TERRIBLE
set(assimp_DIR ${CMAKE_CURRENT_BINARY_DIR}/../external/assimp/)
message("${assimp_DIR}")
find_package(assimp REQUIRED)
if (assimp_FOUND)
  link_directories(${ASSIMP_LIBRARY_DIRS})
  include_directories(${ASSIMP_INCLUDE_DIRS})
  target_link_libraries (turniprenderer-ext ${ASSIMP_LIBRARIES})
endif(assimp_FOUND)
message("Assimp")
message(${ASSIMP_LIBRARY_DIRS})
message(${ASSIMP_LIBRARIES})

add_library(turniprenderer STATIC ${TR_HEADER_FILES} ${TR_SRC_FILES})
target_include_directories(turniprenderer PRIVATE ${TR_PRIVATE_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS})
target_compile_options(turniprenderer PUBLIC "-std=c++1z" "-Wall" "-Werror" "-Wno-unused" "-Wno-unused-but-set-variable" "-g")
target_link_libraries(turniprenderer PRIVATE turniprenderer-ext)

target_include_directories(turniprenderer PRIVATE ${SDL2_INCLUDE_DIRS})

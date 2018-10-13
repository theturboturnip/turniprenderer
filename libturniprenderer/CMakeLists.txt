project(libturniprenderer VERSION 1.0.0 DESCRIPTION "Renderer/Game Engine")
  
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeSources.txt)

add_library(turniprenderer STATIC ${TR_HEADER_FILES} ${TR_SRC_FILES})
# Use the public include dir because when headers are included publicly they will be using this dir and not the private one
target_include_directories(turniprenderer PRIVATE ${TR_PUBLIC_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIRS})
target_compile_options(turniprenderer PUBLIC "-std=c++1z" "-Wall" "-Werror" "-Wno-unused")
target_link_libraries(turniprenderer PRIVATE ${EXTERNAL_LIBRARIES})

target_include_directories(turniprenderer PRIVATE ${SDL2_INCLUDE_DIRS})

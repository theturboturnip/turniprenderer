project(libturniprenderer VERSION 1.0.0 DESCRIPTION "Renderer/Game Engine")
  
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeSources.txt)

add_library(turniprenderer-ext STATIC ${EXTERNAL_SRC_FILES})
target_include_directories(turniprenderer-ext PUBLIC ${EXTERNAL_INCLUDE_DIRS})
target_compile_options(turniprenderer-ext PRIVATE "-std=c++1z" "-Wno-everything") 

add_library(turniprenderer STATIC ${TR_HEADER_FILES} ${TR_SRC_FILES})
target_include_directories(turniprenderer PRIVATE ${TR_PRIVATE_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIRS})
target_compile_options(turniprenderer PUBLIC "-std=c++1z" "-Wall" "-Werror" "-g")
target_link_libraries(turniprenderer PRIVATE turniprenderer-ext)

target_include_directories(turniprenderer PRIVATE ${SDL2_INCLUDE_DIRS})

project (turniprenderer-demo VERSION 1.0.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeSources.txt)

add_executable(demo ${TR_DEMO_SRC_FILES})
target_compile_options(turniprenderer PUBLIC "-std=c++1z" "-Wall" "-Werror" "-Wno-unused-variable")
target_include_directories(demo PRIVATE ${TR_PUBLIC_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIRS})
target_link_libraries(demo turniprenderer ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

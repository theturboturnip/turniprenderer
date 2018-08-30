if (TARGET turniprenderer)
  message("turniprenderer is already defined")
else()
project(libturniprenderer VERSION 1.0.0 DESCRIPTION "Renderer/Game Engine")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FILES} -std=c++1z -Wall -Werror")

add_subdirectory(src)
endif()

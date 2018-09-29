add_library(stb STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.cpp
  )
target_include_directories(stb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_options(stb PRIVATE "-std=c++1z" "-Wno-everything")

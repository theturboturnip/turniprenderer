set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory(assimp)

add_subdirectory(imgui)

add_library(turniprenderer-ext STATIC)
#target_include_directories(turniprenderer-ext PUBLIC ${EXTERNAL_INCLUDE_DIRS})
#target_compile_options(turniprenderer-ext PRIVATE "-std=c++1z" "-Wno-everything")

set(EXTERNAL_LIBRARIES)

# TODO: This is TERRIBLE?
set(assimp_DIR ${CMAKE_CURRENT_BINARY_DIR}/assimp/)
message("${assimp_DIR}")
find_package(assimp REQUIRED)
if (assimp_FOUND)
  #link_directories(turniprenderer-ext ${ASSIMP_LIBRARY_DIRS})
  #target_include_directories(turniprenderer-ext ${ASSIMP_INCLUDE_DIRS})
  set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} ${ASSIMP_LIBRARIES})
  target_link_libraries (turniprenderer-ext ${ASSIMP_LIBRARIES})
endif(assimp_FOUND)

set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} imgui)

set(EXTERNAL_INCLUDE_DIRS
  ${ASSIMP_INCLUDE_DIRS}#${CMAKE_CURRENT_SOURCE_DIR}/assimp/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/glm/include/
  PARENT_SCOPE)
set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} PARENT_SCOPE)

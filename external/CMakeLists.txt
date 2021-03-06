set(EXTERNAL_LIBRARIES)
set(EXTERNAL_INCLUDE_DIRS)

# ASSIMP
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory(assimp)
# TODO: This seems pretty terrible. Is it?
# One would assume the library would be easily accessible through the add_subdirectory call
set(assimp_DIR ${CMAKE_CURRENT_BINARY_DIR}/assimp/)
find_package(assimp REQUIRED)
if (assimp_FOUND)
  set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} ${ASSIMP_LIBRARIES})
  set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS})
endif(assimp_FOUND)

# IMGUI
add_subdirectory(imgui)
set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} imgui)
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/)

# GLM
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/glm/include/)

# STB
add_subdirectory(stb)
set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} stb)
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/stb/include)

# FlatBuffers
add_subdirectory(flatbuffers)

# Setup the parent scope versions
set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} PARENT_SCOPE)
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} PARENT_SCOPE)

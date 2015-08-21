project(CL)
set (PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (PROJECT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/library)
set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

#OpengL
find_package(OpenGL REQUIRED)

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/thirdparty/sdl_cmake")
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
find_package(SDL2 REQUIRED)

#ffmpeg
set (FFMPEG_ROOT ${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg)
set(FFMPEG_LIBRARY ${FFMPEG_ROOT}/lib/avcodec.lib ${FFMPEG_ROOT}/lib/avformat.lib ${FFMPEG_ROOT}/lib/avutil.lib ${FFMPEG_ROOT}/lib/swscale.lib )
set(FFMPEG_INCLUDE_DIR ${FFMPEG_ROOT}/include)

# Compile external dependencies y
add_subdirectory (${CMAKE_SOURCE_DIR}/thirdparty)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

add_subdirectory(CL)
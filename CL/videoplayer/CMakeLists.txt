cmake_minimum_required(VERSION 2.8)
project(CL_videoplayer)

set (PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (PROJECT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)
set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


include_directories(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/thirdparty/glew-1.9.0/include/
	${SDL2_INCLUDE_DIR}
	${FFMPEG_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/thirdparty/glm-0.9.4.0/
	${OVR_INCLUDE}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set(HEADER_LOCATION ${CMAKE_SOURCE_DIR}/include)
add_library(CL_VideoLibraryFFMPEG 
	CL_VideoLibraryFfmpegImpl.cpp 
	${HEADER_LOCATION}/videoplayer/CL_VideoLibrary.hpp
)
target_link_libraries(CL_VideoLibraryFFMPEG 
	${FFMPEG_LIBRARY}
)

add_executable(CL_VideoPlayer 
	CL_VideoPlayerApp.cpp
	common/shader.cpp
)
target_link_libraries(CL_VideoPlayer 
	CL_VideoLibraryFFMPEG 
	${OPENGL_LIBRARY} 
	GLEW_190 
	${SDL2_LIBRARY} 
	CL_Shapes	
	${OVR_LIBS}
)

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET CL_VideoLibraryFFMPEG POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_LIBRARY_DIR}/videoplayer
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_VideoLibraryFFMPEG${CMAKE_STATIC_LIBRARY_SUFFIX}" "${PROJECT_LIBRARY_DIR}/videoplayer/"
)

add_custom_command(
	TARGET CL_VideoPlayer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_LIBRARY_DIR}/videoplayer
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_VideoPlayer${CMAKE_EXECUTABLE_SUFFIX}" "${PROJECT_BINARY_DIR}/videoplayer/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
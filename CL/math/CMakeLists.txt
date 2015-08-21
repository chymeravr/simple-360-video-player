cmake_minimum_required(VERSION 2.8)
project(CL_Math)

set (PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (PROJECT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)
set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

#Directory containing APIs of math to be exposed to outside world
include_directories(
	${CMAKE_SOURCE_DIR}/include
)
set(HEADER_LOCATION ${CMAKE_SOURCE_DIR}/include)
add_library(CL_Math CL_LinearAlgebra.cpp ${HEADER_LOCATION}/math/CL_LinearAlgebra.hpp)

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET CL_Math POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_LIBRARY_DIR}/math
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_Math${CMAKE_STATIC_LIBRARY_SUFFIX}" "${PROJECT_LIBRARY_DIR}/math/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )


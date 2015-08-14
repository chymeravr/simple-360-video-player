cmake_minimum_required(VERSION 2.8)
project(CL_Test)

set (PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (PROJECT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)
set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${CMAKE_SOURCE_DIR}/include)
set (LIB_ALL
	${CMAKE_SOURCE_DIR}/lib/math/CL_Math.lib)
add_executable(CL_LinearAlgebraTest CL_LinearAlgebraTest.cpp)
target_link_libraries(CL_LinearAlgebraTest ${LIB_ALL})

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET CL_LinearAlgebraTest POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/test
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_LinearAlgebraTest${CMAKE_EXECUTABLE_SUFFIX}" "${PROJECT_BINARY_DIR}/test/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
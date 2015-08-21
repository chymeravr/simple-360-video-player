cmake_minimum_required(VERSION 2.8)
project(CL_Shapes)

set (PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (PROJECT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)
set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(
	${CMAKE_SOURCE_DIR}/include
)

set(HEADER_LOCATION ${CMAKE_SOURCE_DIR}/include)
add_library(CL_Shapes CL_Shape.cpp ${HEADER_LOCATION}/shapes/CL_Shape.hpp CL_Sphere.cpp ${HEADER_LOCATION}/shapes/CL_Sphere.hpp)
target_link_libraries(CL_Shapes CL_Math)

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET CL_Shapes POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_LIBRARY_DIR}/shapes
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_Shapes${CMAKE_STATIC_LIBRARY_SUFFIX}" "${PROJECT_LIBRARY_DIR}/shapes/"
)
#add_custom_command(
#	TARGET CL_Shapes POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_INCLUDE_DIR}/shapes
#	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/CL_Shape.hpp" "${PROJECT_INCLUDE_DIR}/shapes/"
#)
#add_custom_command(
#	TARGET CL_Shapes POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/CL_Sphere.hpp" "${PROJECT_INCLUDE_DIR}/shapes/"
#)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
cmake_minimum_required(VERSION 2.8)
project(CL_Test)

set (PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (PROJECT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)
set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(GMOCK_DIR $ENV{GMOCK}
    CACHE PATH "The path to the GoogleMock test framework.")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # force this option to ON so that Google Test will use /MD instead of /MT
    # /MD is now the default for Visual Studio, so it should be our default, too
    option(gtest_force_shared_crt
           "Use shared (DLL) run-time lib even when Google Test is built as static lib."
           ON)
elseif (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
endif()

add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(SYSTEM 	${GMOCK_DIR}/gtest/include
							${GMOCK_DIR}/include)
					

add_executable(CL_LinearAlgebraTest CL_LinearAlgebraTest.cpp)
target_link_libraries(CL_LinearAlgebraTest CL_Math)

add_executable(CL_SphereTest CL_SphereTest.cpp)
target_link_libraries(CL_SphereTest CL_Shapes gmock_main)
add_test(CL_SphereTest CL_SphereTest)

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET CL_LinearAlgebraTest POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/test
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_LinearAlgebraTest${CMAKE_EXECUTABLE_SUFFIX}" "${PROJECT_BINARY_DIR}/test/"
)
add_custom_command(
   TARGET CL_SphereTest POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/test
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/CL_SphereTest${CMAKE_EXECUTABLE_SUFFIX}" "${PROJECT_BINARY_DIR}/test/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
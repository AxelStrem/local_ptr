cmake_minimum_required (VERSION 2.8)
project (local_ptr)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
enable_testing()

include_directories(include)
set(CATCH_HEADER_PATH ${PROJECT_SOURCE_DIR}/test/catch.hpp)

add_executable(runTest ${PROJECT_SOURCE_DIR}/test/test.cpp ${CATCH_HEADER_PATH})

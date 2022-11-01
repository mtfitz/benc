cmake_minimum_required(VERSION 3.1.0)
project(benc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    find_package(getopt)
endif()

add_subdirectory(src)

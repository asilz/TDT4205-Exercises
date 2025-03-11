cmake_minimum_required(VERSION 3.10)
project(Exercises VERSION 1.0 LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building for release")
endif()
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Exercise1)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Exercise2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Exercise3)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Exercise4)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Exercise5)



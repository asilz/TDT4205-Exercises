cmake_minimum_required(VERSION 3.10)
project(Exercises)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Exercise1)



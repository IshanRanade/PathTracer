cmake_minimum_required(VERSION 3.11)
project(pathtracer)

find_library(OPENCL OpenCL)
if (NOT OPENCL)
    message(FATAL_ERROR "AudioUnit not found")
endif()

set (CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "*.cpp")

include_directories(include)

add_executable(pathtracer main.cpp)
target_link_libraries(pathtracer ${OPENCL})
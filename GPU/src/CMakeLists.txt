cmake_minimum_required(VERSION 3.11)
project(pathtracer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/")

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -g -I.")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenCL")

file(GLOB SOURCES "*.cpp")

include_directories(include)

add_executable(pathtracer main.cpp)

target_compile_options(pathtracer PUBLIC "-F" OpenCL)
target_include_directories (pathtracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pathtracer "-framework OpenCL")
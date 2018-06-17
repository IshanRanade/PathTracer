cmake_minimum_required(VERSION 3.11)
project(pathtracer)

find_package(OpenCL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets Qml Quick Concurrent REQUIRED)

include_directories(${Qt5Widgets_INCLUDES})

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS mainwindow.ui)

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenCL")

file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(include)

add_executable(pathtracer ${SOURCES})

target_compile_options(pathtracer PUBLIC "-F" OpenCL)
target_link_libraries(pathtracer Qt5::Widgets)
target_include_directories (pathtracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

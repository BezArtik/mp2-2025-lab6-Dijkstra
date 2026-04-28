cmake_minimum_required(VERSION 3.30...4.1)
project(dijkstra)

option(BUILD_SAMPLES "Build sample applications" ON)
option(BUILD_TESTS "Build tests" ON)

add_library(containers INTERFACE)
target_include_directories(containers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(containers INTERFACE cxx_std_20)

add_subdirectory(src)

add_library(project::containers ALIAS containers)


if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
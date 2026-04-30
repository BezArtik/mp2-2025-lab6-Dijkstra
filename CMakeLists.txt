cmake_minimum_required(VERSION 3.30...4.1)
project(dijkstra)

option(BUILD_SAMPLES "Build sample applications" ON)
option(BUILD_TESTS "Build tests" ON)

function(add_interface name)
    add_library(${name} INTERFACE)
    target_include_directories(${name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${name} INTERFACE cxx_std_20)
    add_library(project::::${name} ALIAS ${name})
endfunction()

add_interface(containers)
add_interface(heaps)
target_link_libraries(heaps INTERFACE containers)

add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
cmake_minimum_required(VERSION 3.30...4.1)

include(FetchContent)
set(BUILD_GMOCK ON CACHE BOOL "Build GMock" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable install" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "Force shared CRT" FORCE)
FetchContent_Declare(
	GoogleTest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(GoogleTest)

add_executable(tests)

target_sources(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

target_link_libraries(tests
	PRIVATE
    containers
	GTest::gtest_main
	GTest::gmock
)

target_include_directories(tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(tests)

cmake_minimum_required(VERSION 3.22)
project(hub75_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
endif()

find_package(GTest REQUIRED)
enable_testing()

add_executable(hub75_tests test_types.cpp test_framebuffer.cpp)
target_include_directories(hub75_tests PRIVATE ${CMAKE_SOURCE_DIR}/../include)

target_link_libraries(hub75_tests PRIVATE GTest::GTest GTest::Main)
include(GoogleTest)
gtest_discover_tests(hub75_tests)

# Header check target for clang-tidy (compile-only, not linked to GTest)
add_library(hub75_header_check OBJECT check_all_headers.cpp)
target_include_directories(hub75_header_check PRIVATE ${CMAKE_SOURCE_DIR}/../include)

cmake_minimum_required(VERSION 3.22)
project(hub75_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
enable_testing()

add_executable(hub75_tests test_types.cpp)
target_include_directories(hub75_tests PRIVATE ${CMAKE_SOURCE_DIR}/../include)

target_link_libraries(hub75_tests PRIVATE GTest::GTest GTest::Main)
include(GoogleTest)
gtest_discover_tests(hub75_tests)

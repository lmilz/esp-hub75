cmake_minimum_required(VERSION 3.22)
project(hub75_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

# Jedes .cpp in diesem Ordner wird ein eigenes Executable
file(GLOB EXAMPLE_SOURCES "*.cpp")
foreach(src ${EXAMPLE_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/../../include
        ${CMAKE_SOURCE_DIR}/../../apps)
    target_link_libraries(${name} PRIVATE SDL2::SDL2)
endforeach()

# HUB75 LED Matrix Library for ESP32
#
# To select the application, set the APP variable:
#   idf.py -DAPP=matrix build
#   idf.py -DAPP=gol build
# Copyright (c) 2025 Lars Milz

cmake_minimum_required(VERSION 3.22)

# Default app selection
if(NOT DEFINED APP)
  set(APP "matrix" CACHE STRING "Application to build (matrix or game of line)")
endif()

message(STATUS "Building application: ${APP}")

# Set the main source file based on selected app
if(APP STREQUAL "matrix")
  set(MAIN_SRC "apps/matrix/main.c")
elseif(APP STREQUAL "gol")
  set(MAIN_SRC "apps/game_of_life/main.c")
else()
  message(FATAL_ERROR "Unknown App: ${APP}. Use 'matrix' or 'gol'")
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(hub75_${APP})

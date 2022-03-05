cmake_minimum_required(VERSION 3.5)

project(vlang LANGUAGES CXX)

#   CXX Language option
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
set(VLANG_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(tools)

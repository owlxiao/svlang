cmake_minimum_required(VERSION 3.5)

project(svlang LANGUAGES CXX)

#   CXX Language option
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
set(VLANG_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

#   llvm 
find_package(LLVM REQUIRED HINTS "$LLVM_CMAKE_PATH")

message(STATUS "Using LLVMConfig.cmake in:" ${LLVM_DIR})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(AddLLVM)

include_directories("${LLVM_INCLUDE_DIRS}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

#   Project include
include(AddSVlang)

include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
#   
add_subdirectory(lib)
add_subdirectory(tools)


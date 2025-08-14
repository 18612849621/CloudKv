cmake_minimum_required(VERSION 3.17)
# compile option
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-sign-compare)
add_compile_options(-Wno-unused-function)
# cpp
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# code jmp or use "cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .."
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  CloudKv
  VERSION 1.0
  LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# third_party
add_subdirectory(third_party/gflags)
add_subdirectory(third_party/glog)
add_subdirectory(third_party/googletest)
# src
add_subdirectory(memtable)
add_subdirectory(utils)
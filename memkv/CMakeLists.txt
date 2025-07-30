cmake_minimum_required(VERSION 3.17)
project(
  MemKV
  VERSION 1.0
  LANGUAGES C CXX)

# include cmake
include(cmake/environment.cmake)
include(cmake/compile_options.cmake)

# static lib
aux_source_directory(src SRC_LIST)
add_library(memkv_lib STATIC ${SRC_LIST})

# bin
add_executable(main_test test/main.cc)

# include
target_include_directories(memkv_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# link
target_link_libraries(main_test PRIVATE glog gflags gtest memkv_lib)

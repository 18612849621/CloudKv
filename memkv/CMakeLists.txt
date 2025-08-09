cmake_minimum_required(VERSION 3.17)

project(
  MemKV
  VERSION 1.0
  LANGUAGES CXX)

# static lib
aux_source_directory(src SRC_LIST)
# memkv_lib
add_library(memkv_lib STATIC ${SRC_LIST})
target_include_directories(memkv_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(memkv_lib PRIVATE glog gflags)

# TEST
add_executable(main_test test/main_test.cc)
target_link_libraries(main_test PRIVATE glog gflags gtest memkv_lib)

add_executable(file_system_test test/file_system_test.cc)
target_link_libraries(file_system_test PRIVATE glog gflags gtest memkv_lib)

cmake_minimum_required(VERSION 3.17)
project(
  CloudKv
  VERSION 1.0
  LANGUAGES C CXX)

# third_party
add_subdirectory(third_party/gflags)
add_subdirectory(third_party/glog)
add_subdirectory(third_party/googletest)
# framework
add_subdirectory(memkv)

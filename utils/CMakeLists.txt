# utils
add_library(utils STATIC file_system.cc)
target_link_libraries(utils PUBLIC glog gflags)

# test
add_executable(file_system_test file_system_test.cc)
target_link_libraries(file_system_test PRIVATE gtest utils)

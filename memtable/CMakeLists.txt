# memtable
add_library(memtable STATIC mem_pool.cc mem_slab_pool32.cc
                                shm_pool.cc)
target_link_libraries(memtable PUBLIC utils)

# test
add_executable(mem_pool_test mem_pool_test.cc)
target_link_libraries(mem_pool_test PRIVATE gtest memtable)
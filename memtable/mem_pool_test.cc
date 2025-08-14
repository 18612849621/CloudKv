#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <new>

#include "memtable/shm_pool.h"
#define SHM_FILE_PATH "/dev/shm"
DEFINE_int64(mem_chunck_max_size, 4 * 1024 * 1024, "Shm block max size.");
TEST(AllocateTest, alloc_block) {
    cloudkv::memtable::ShmPool* shm_pool = new cloudkv::memtable::ShmPool(SHM_FILE_PATH, 0);
    void* block_data_addr = shm_pool->Malloc(FLAGS_mem_chunck_max_size);
    ;
    shm_pool->Free(block_data_addr);
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

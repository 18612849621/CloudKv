#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <new>

#include "shm_pool.h"
#define SHM_FILE_PATH "/dev/shm"
DEFINE_int64(mem_chunck_max_size, 4 * 1024 * 1024, "Shm block max size.");
TEST(AllocateTest, alloc_block) {
    cloudkv::base::ShmPool* shm_pool = new cloudkv::base::ShmPool(SHM_FILE_PATH, 0);
    shm_pool->Malloc(FLAGS_mem_chunck_max_size);
    ;
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

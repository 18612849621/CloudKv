#include "file_system.h"

#include <glog/logging.h>
#include <gtest/gtest.h>
namespace cloudkv {
namespace base {
TEST(FileSystem, create_file) {
    const std::string test_dir = "/dev/shm/demo";  // multi-level directory

    // 1. ​Recursively create directories​
    if (FileSystem::CreateDirectory(test_dir)) {
        LOG(INFO) << "[SUCCESS] The directory has been created: " << test_dir;
    } else {
        LOG(INFO) << "[FAILED] Directory creation failed: " << test_dir;
    }

    // 2. Check if directory exists
    if (FileSystem::CheckPathExist(test_dir)) {
        LOG(INFO) << "[Detection] Directory exists: " << test_dir;
    }

    // 3. Recursively delete directories
    if (FileSystem::RemoveDirectory("./test")) {  // delete parent folder
        LOG(INFO) << "[CLEANUP] Directory deleted: ./test";
    }
    const char* test_path = "/dev/shm/demo/0/1";
    LOG(INFO) << "Parent folder name is: " << FileSystem::DirName(test_path);
}

}  // namespace base
}  // namespace cloudkv
int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
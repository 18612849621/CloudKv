#include <fcntl.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main() {
    LOG(INFO) << "test";
    //   size_t file_size = 1 << 20; // 1MB
    //   int fd = open("data.txt", O_RDWR | O_CREAT, 0666);
    //   int status = fallocate(fd, 0, 0, file_size);
    //   void *addr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED,
    //   fd, 0); memcpy(addr, "HELLO", 5);        // 直接修改文件内容 msync(addr,
    //   file_size, MS_SYNC); // 同步到磁盘 munmap(addr, file_size);
}
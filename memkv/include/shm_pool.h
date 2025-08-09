#pragma once

#include <stdint.h>

#include <string>

#include "mem_pool.h"
namespace cloudkv {
namespace base {

class ShmPool : public MemPool {
   public:
    ShmPool() = default;
    explicit ShmPool(std::string dir_name, uint32_t part_id)
        : dir_name_(dir_name), part_id_(part_id) {}
    ~ShmPool() = default;

    void *Malloc(uint64_t size, int *fd = nullptr) override;
    void Free(void *ptr) override;

   public:
    struct ShmBlock {
        std::string file_name;
        uint64_t size = 0;
        int fd = -1;
        int index = -1;
        void *data = nullptr;
        ShmBlock *next = nullptr;
    };

   private:
    inline std::string CurBlockFileName() const {
        return (dir_name_ + "/" + std::to_string(current_block_index_) + "/" +
                std::to_string(current_block_index_));
    }

    bool LoadBlock(ShmBlock *block);
    bool VMMap(ShmBlock *block);
    bool CheckFileExistAndSize(const std::string &file_name, const size_t &file_size);
    bool CreateFile(const std::string &file_name, const size_t &file_size);

   private:
    ShmBlock *head_;
    // shm file
    std::string dir_name_ = "";
    uint32_t part_id_ = 0;
    uint32_t current_block_index_ = 0;
};

}  // namespace base
}  // namespace cloudkv
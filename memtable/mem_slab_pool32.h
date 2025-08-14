#pragma once
#include "memtable/mem_pool.h"
#include "memtable/shm_pool.h"
namespace cloudkv {
namespace memtable {

class MemSlabPool32 {
   public:
    MemSlabPool32() = default;
    ~MemSlabPool32() = default;

    void Initialize(std::string shm_path, uint32_t part_id, uint32_t max_block_num);

   private:
    MemoryPool* mem_pool_;
};
}  // namespace memtable
}  // namespace cloudkv
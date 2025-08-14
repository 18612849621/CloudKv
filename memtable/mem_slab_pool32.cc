#include "memtable/mem_slab_pool32.h"

namespace cloudkv {
namespace memtable {

void MemSlabPool32::Initialize(std::string shm_path, uint32_t part_id, uint32_t max_block_num) {
    mem_pool_ = new ShmPool(shm_path, part_id);
}
}  // namespace memtable
}  // namespace cloudkv
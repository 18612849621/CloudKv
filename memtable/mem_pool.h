#pragma once

#include <stdint.h>
namespace cloudkv {
namespace memtable {

class MemoryPool {
   public:
    MemoryPool() = default;
    virtual ~MemoryPool() = default;

    virtual void *Malloc(uint64_t size, int *fd = nullptr);
    virtual void Free(void *ptr);
};

}  // namespace memtable
}  // namespace cloudkv
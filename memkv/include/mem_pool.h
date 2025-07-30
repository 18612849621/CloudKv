#pragma once

#include <stdint.h>
namespace cloudkv {
namespace base {

class MemPool {
   public:
    MemPool() = default;
    virtual ~MemPool() = default;

    virtual void *Malloc(uint64_t size, int *fd = nullptr);
    virtual void Free(void *ptr);
};

}  // namespace base
}  // namespace cloudkv
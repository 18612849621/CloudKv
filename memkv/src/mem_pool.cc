#include "mem_pool.h"

#include <new>

namespace cloudkv {
namespace base {

void* MemPool::Malloc(uint64_t size, int* fd) {
    auto ptr = new (std::nothrow) char[size]();
    return ptr;
}

void MemPool::Free(void* ptr) {
    if (ptr != nullptr) {
        delete[] reinterpret_cast<char*>(ptr);
        ptr = nullptr;
    }
}

}  // namespace base
}  // namespace cloudkv
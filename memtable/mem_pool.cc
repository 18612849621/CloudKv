#include "memtable/mem_pool.h"

#include <new>

namespace cloudkv {
namespace memtable {

void* MemoryPool::Malloc(uint64_t size, int* fd) {
    auto ptr = new (std::nothrow) char[size]();
    return ptr;
}

void MemoryPool::Free(void* ptr) {
    if (ptr != nullptr) {
        delete[] reinterpret_cast<char*>(ptr);
        ptr = nullptr;
    }
}

}  // namespace memtable
}  // namespace cloudkv
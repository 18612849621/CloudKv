#include "shm_pool.h"

#include <glog/logging.h>

#include <fstream>
#include <new>

#include "file_system.h"

namespace cloudkv {
namespace base {

void* ShmPool::Malloc(uint64_t size, int* fd) {
    ShmBlock* block = new (std::nothrow) ShmBlock();
    if (block == nullptr) {
        LOG(ERROR) << "Create block " << current_block_index_ << " error.";
    }
    block->file_name = CurBlockFileName();
    block->size = size;
    block->index = current_block_index_++;
    if (!LoadResourceForBlock(block)) {
        LOG(ERROR) << "Load resource for block " << current_block_index_ << " error.";
    }
    LOG(INFO) << "Create block " << current_block_index_ << " success.";
    return block->data;
}

void ShmPool::Free(void* ptr) {
    // TODO(panyuchen): find block and unmap
}

bool ShmPool::LoadResourceForBlock(ShmBlock* block) {
    // LOG(INFO) << "Block file name is: " << block->file_name;
    if (!FileSystem::DirectoryExists(block->file_name)) {
        if (!FileSystem::CreateDirectory(FileSystem::DirName(block->file_name))) {
            LOG(FATAL) << "Create directory for Block \"" << block->file_name << "\" failed";
        }
        LoadFile(block->file_name, block->size);
    } else {
        LOG(INFO) << "File " << block->file_name << " is existed.";
    }

    // FileSystem::CreateDirectory(block->file_name);
    return true;
}

bool ShmPool::LoadFile(const std::string& file_name, const size_t& file_size) {
    // NOTE(panyuchen): To increase the stability of writing files, load them by chunck
    std::ofstream output(file_name);
    if (!output) {
        LOG(ERROR) << "Unable to open file " << file_name;
        return false;
    }
    const uint32_t buffer_size = 4 * 1024;
    char buffer[buffer_size] = {0};
    for (int i = 0; i < file_size / buffer_size; ++i) {
        output.write(buffer, buffer_size);
        if (!output.good()) {
            LOG(ERROR) << "Init file " << file_name << "occur error at offset " << i * buffer_size
                       << ".";
            return false;
        }
    }
    if (file_size % buffer_size != 0) {
        output.write(buffer, file_size % buffer_size);
        if (!output.good()) {
            LOG(ERROR) << "Init file " << file_name << "occur error at offset "
                       << (file_size / buffer_size) * buffer_size << ".";
            return false;
        }
    }
    return true;
}

}  // namespace base
}  // namespace cloudkv
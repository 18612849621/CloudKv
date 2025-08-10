#include "shm_pool.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <sys/mman.h>

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
    if (!LoadBlock(block)) {
        LOG(ERROR) << "Load block " << current_block_index_ << " error.";
    }
    LOG(INFO) << "Create block " << current_block_index_ << " success.";
    return block->data;
}

void ShmPool::Free(void* ptr) {
    // TODO(panyuchen): find block and unmap
}

bool ShmPool::LoadBlock(ShmBlock* block) {
    LOG(INFO) << "Block file name is: " << block->file_name;
    // no need to mmap
    if (block->data != nullptr && block->fd >= 0) return true;
    const std::string file_name = block->file_name;
    const size_t block_size = block->size;

    if (!CheckFileExistAndSize(file_name, block_size)) {
        return false;
    }
    if (!FileSystem::CheckPathExist(file_name)) {
        if (!FileSystem::CreateDirectory(FileSystem::DirName(file_name))) {
            LOG(FATAL) << "Create directory for Block \"" << block->file_name << "\" failed";
        }
        CreateFile(file_name, block_size);
    } else {
        LOG(INFO) << "File " << file_name << " is existed.";
    }

    if (!CheckFileExistAndSize(file_name, block_size)) {
        return false;
    }

    return VMMap(block);
}
bool ShmPool::VMMap(ShmBlock* block) {
    auto fd = open(block->file_name.c_str(), O_RDWR);
    if (fd <= 0) {
        LOG(ERROR) << "Block " << block->file_name << " map file open failed, " << strerror(errno);
        return false;
    }
    void* last_ptr = block->data;
    block->data = mmap(nullptr, block->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (block->data == MAP_FAILED) {
        LOG(ERROR) << "Failed to map file:" << block->file_name << ", size: " << block->size
                   << ", part_id:" << part_id_ << ", ptr:" << block->data
                   << ", last_ptr:" << last_ptr << ", errno:" << errno
                   << ", msg:" << strerror(errno);
        return false;
    }
    block->fd = fd;
    LOG(INFO) << "Create ShmFile: " << block->file_name << ", size: " << block->size
              << ", part_id:" << part_id_ << ", ptr:" << block->data << ", last_ptr:" << last_ptr;
    return true;
}
bool ShmPool::CheckFileExistAndSize(const std::string& file_name, const size_t& file_size) {
    // NOTE(panyuchen): if file path or size invalid, then to be delete.
    if (FileSystem::CheckPathExist(file_name)) {
        size_t get_file_size;
        if (!FileSystem::GetFileSize(file_name, &get_file_size)) {
            LOG(ERROR) << "File " << file_name << " is invalid.";
            FileSystem::DeleteFile(file_name);
            return false;
        }

        if (get_file_size != file_size) {
            LOG(ERROR) << "File " << file_name << " size " << get_file_size
                       << " bytes is not equal need size " << file_size << " bytes.";
            FileSystem::DeleteFile(file_name);
            return false;
        }
    }
    return true;
}

bool ShmPool::CreateFile(const std::string& file_name, const size_t& file_size) {
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
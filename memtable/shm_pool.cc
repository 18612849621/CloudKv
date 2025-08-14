#include "memtable/shm_pool.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <sys/mman.h>

#include <new>

#include "utils/file_system.h"

namespace cloudkv {
namespace memtable {

void* ShmPool::Malloc(uint64_t size, int* fd) {
    ShmBlock* new_block = new (std::nothrow) ShmBlock();
    if (new_block == nullptr) {
        LOG(ERROR) << "Create block " << current_block_index_ << " error.";
    }
    new_block->file_name = GetCurShmBlockFileName();
    new_block->size = size;
    new_block->index = current_block_index_;
    if (!LoadShmBlock(new_block)) {
        delete new_block;
        LOG(ERROR) << "Load block " << current_block_index_ << " error.";
        return nullptr;
    }
    // NOTE(panyuchen): Linked list update by front insertion.
    new_block->next = head_;
    head_ = new_block;
    current_block_index_++;
    if (fd) {
        *fd = new_block->fd;
    }
    return new_block->data;
}

void ShmPool::Free(void* ptr) {
    ShmBlock* p = head_;
    while (p != nullptr) {
        if (ptr == p->data) {
            break;
        }
        p = p->next;
    }
    StoreShmBlock(p);
}

void ShmPool::StoreShmBlock(ShmBlock* block) {
    if (block == nullptr) return;
    // NOTE(panyuchen): Only store block data, no need to update linked list.
    if (block->fd >= 0 && block->data != nullptr) {
        msync(block->data, block->size, MS_SYNC);
        munmap(block->data, block->size);
        close(block->fd);

        LOG(INFO) << "[Unmap Info] shm file:" << block->file_name << ", fd:" << block->fd
                  << ", size:" << block->size << ", part_id:" << part_id_
                  << ", ptr:" << block->data;
        block->fd = -1;
        block->data = nullptr;
    }
}

bool ShmPool::LoadShmBlock(ShmBlock* block) {
    LOG(INFO) << "Shm block " << block->index << " file name is: " << block->file_name;
    // No need to mmap
    if (block->data != nullptr && block->fd >= 0) return true;
    const std::string file_name = block->file_name;
    const size_t block_size = block->size;
    // NOTE(panyuchen): double-check file valid.
    if (!CheckFileExistAndSize(file_name, block_size)) {
        return false;
    }
    if (!utils::FileSystem::CheckPathExist(file_name)) {
        if (!utils::FileSystem::CreateDirectory(utils::FileSystem::GetDirName(file_name))) {
            LOG(FATAL) << "Create directory for Shm block \"" << block->file_name << "\" failed";
        }
        CreateFileForShmBlock(file_name, block_size);
    } else {
        LOG(INFO) << "Shm file " << file_name << " is existed.";
    }

    if (!CheckFileExistAndSize(file_name, block_size)) {
        return false;
    }

    return MapFileToMemoryForShmBlock(block);
}
bool ShmPool::MapFileToMemoryForShmBlock(ShmBlock* block) {
    auto fd = open(block->file_name.c_str(), O_RDWR);
    if (fd <= 0) {
        LOG(ERROR) << "Shm block " << block->file_name << " map file open failed, "
                   << strerror(errno);
        return false;
    }
    block->data = mmap(nullptr, block->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (block->data == MAP_FAILED) {
        LOG(ERROR) << "Failed to map file:" << block->file_name << ", size: " << block->size
                   << ", part_id:" << part_id_ << ", ptr:" << block->data << ", errno:" << errno
                   << ", msg:" << strerror(errno);
        return false;
    }
    block->fd = fd;
    LOG(INFO) << "[Mmap Info] shm file:" << block->file_name << ", size:" << block->size
              << ", part_id:" << part_id_ << ", ptr:" << block->data;
    return true;
}
bool ShmPool::CheckFileExistAndSize(const std::string& file_name, const size_t& file_size) {
    // NOTE(panyuchen): if file path or size invalid, then to be delete.
    if (utils::FileSystem::CheckPathExist(file_name)) {
        size_t get_file_size;
        if (!utils::FileSystem::GetFileSize(file_name, &get_file_size)) {
            LOG(ERROR) << "Shm file " << file_name << " is invalid.";
            utils::FileSystem::DeleteFile(file_name);
            return false;
        }

        if (get_file_size != file_size) {
            LOG(ERROR) << "Shm file " << file_name << " size " << get_file_size
                       << " bytes is not equal need size " << file_size << " bytes.";
            utils::FileSystem::DeleteFile(file_name);
            return false;
        }
    }
    return true;
}

bool ShmPool::CreateFileForShmBlock(const std::string& file_name, const size_t& file_size) {
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
}  // namespace memtable
}  // namespace cloudkv
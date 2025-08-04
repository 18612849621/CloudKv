#include <dirent.h>    // opendir, readdir, closedir
#include <sys/stat.h>  // mkdir, stat, S_ISDIR
#include <unistd.h>    // unlink, rmdir

#include <cstring>  // strcmp
#include <iostream>
#include <string>
#include <vector>
namespace cloudkv {
namespace base {
class FileSystem {
   public:
    // Create a single-level directory (returns false if the directory already exists)
    inline static bool CreateSingleLevelDirectory(const std::string& path) {
        if (DirectoryExists(path)) {
            return false;  // Directory already exists
        }
        // Set permissions: user read, write and execute, group and others read and execute (0755)
        return mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0;
    }

    // Recursively create mutil-level folder (like `mkdir -p`)
    inline static bool CreateDirectory(const std::string& path) {
        if (path.empty()) return false;

        std::string currentPath;
        for (size_t pos = 0; pos < path.size(); ++pos) {
            currentPath += path[pos];
            // Create directories when encountering a separator or the end of the path​
            if (path[pos] == '/' || pos == path.size() - 1) {
                if (!DirectoryExists(currentPath) && !CreateSingleLevelDirectory(currentPath)) {
                    return false;  // Create foler failed
                }
            }
        }
        return true;
    }

    // Check if directory exists
    inline static bool DirectoryExists(const std::string& path) {
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            return false;  // Path can't be accessed
        }
        return S_ISDIR(info.st_mode);  // Check is a directory
    }

    // Recursively delete directory and data (like `rm -rf`)
    static bool RemoveDirectory(const std::string& path) {
        if (!DirectoryExists(path)) return false;

        DIR* dir = opendir(path.c_str());
        if (!dir) return false;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            // Skip "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::string fullPath = path + "/" + entry->d_name;
            if (entry->d_type == DT_DIR) {
                RemoveDirectory(fullPath);  // Recursively delete sub-directory
            } else {
                unlink(fullPath.c_str());  // Delete file
            }
        }
        closedir(dir);
        return rmdir(path.c_str()) == 0;  // delete empty directory
    }

    inline static std::string DirName(const std::string& path) {
        if (path.empty()) return ".";  // Handle empty path case

        size_t end_pos = path.size() - 1;

        // Skip trailing separators (except root '/')
        while (end_pos > 0 && (path[end_pos] == '/' || path[end_pos] == '\\')) {
            end_pos--;
        }

        // Find last path separator
        size_t last_sep = path.find_last_of("/\\", end_pos);

        if (last_sep == std::string::npos) {
            return ".";  // No parent (single component path)
        }

        // Handle root directory cases
        if (last_sep == 0) {
            return path.substr(0, 1);  // Return root "/" (POSIX) or drive "C:\\" (Windows)
        }

        return path.substr(0, last_sep);  // Return parent path
    }
};
}  // namespace base
}  // namespace cloudkv
#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace minigit {

class FileSystem {
    public:
        std::vector<std::string> listWorkingFiles(const std::filesystem::path& projectRoot) const;
        bool exists(const std::filesystem::path& path) const;
        std::string readBytes(const std::filesystem::path& path) const;
        void remove(const std::filesystem::path& path) const;
};

} // namespace minigit

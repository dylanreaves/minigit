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
        void copy(const std::filesystem::path& source, const std::filesystem::path& target) const;
        void createDirectories(const std::filesystem::path& directory) const;
        void writeText(const std::filesystem::path& path, const std::string& text) const;
        
};

} // namespace minigit

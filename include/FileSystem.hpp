#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace minigit {

class FileSystem {
    public:
        std::vector<std::string> listWorkingFiles(const std::filesystem::path& projectRoot) const;
};

} // namespace minigit

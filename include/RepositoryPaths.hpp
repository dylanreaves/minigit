#pragma once
#include <filesystem>
#include <string>

namespace minigit {

class RepositoryPaths {
    public:
        explicit RepositoryPaths(const std::filesystem::path& root);
        const std::filesystem::path& projectRoot() const;
        const std::filesystem::path headFile() const;
        std::string safeRelativeFile(const std::string& argument) const;
    private:
        std::filesystem::path rootPath;
};

} // namespace minigit

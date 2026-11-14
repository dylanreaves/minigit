#pragma once
#include <filesystem>

namespace minigit {

class RepositoryPaths {
    public:
        explicit RepositoryPaths(const std::filesystem::path& root);
        const std::filesystem::path& projectRoot() const;

    private:
        std::filesystem::path rootPath;
};

} // namespace minigit

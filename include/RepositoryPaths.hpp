#pragma once
#include <filesystem>
#include <string>

namespace minigit {

class RepositoryPaths {
<<<<<<< HEAD
public:
    explicit RepositoryPaths(const std::filesystem::path& root);

    std::string safeRelativeFile(const std::string& argument) const;

    std::filesystem::path metadataDirectory() const;

private:
    std::filesystem::path root_;
};

}
=======
    public:
        explicit RepositoryPaths(const std::filesystem::path& root);
        const std::filesystem::path& projectRoot() const;
        std::string safeRelativeFile(const std::string& argument) const;
    private:
        std::filesystem::path rootPath;
};

} // namespace minigit
>>>>>>> origin/main

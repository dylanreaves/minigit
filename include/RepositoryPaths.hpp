#pragma once
#include <filesystem>
#include <string>

namespace minigit {

class RepositoryPaths {
public:
    explicit RepositoryPaths(const std::filesystem::path& root);

    std::string safeRelativeFile(const std::string& argument) const;

    std::filesystem::path metadataDirectory() const;

private:
    std::filesystem::path root_;
};

}
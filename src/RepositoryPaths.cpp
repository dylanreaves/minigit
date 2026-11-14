#include "RepositoryPaths.hpp"

namespace minigit {

RepositoryPaths::RepositoryPaths(const std::filesystem::path& root)
    : rootPath(root) {
}

const std::filesystem::path& RepositoryPaths::projectRoot() const {
    return rootPath;
}

} // namespace minigit

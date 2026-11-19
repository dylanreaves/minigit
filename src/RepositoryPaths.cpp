#include "RepositoryPaths.hpp"
#include <filesystem>
#include <stdexcept>

namespace minigit {

RepositoryPaths::RepositoryPaths(const std::filesystem::path& root)
    : root_(root) {
}

std::filesystem::path RepositoryPaths::metadataDirectory() const {
    return root_ / "minigit";
}

std::string RepositoryPaths::safeRelativeFile(const std::string& argument) const {
    namespace fs = std::filesystem;

    fs::path input(argument);

    if (input.is_absolute()) {
        throw std::invalid_argument("Absolute paths are not allowed");
    }

    fs::path root = fs::weakly_canonical(root_);
    fs::path candidate = fs::weakly_canonical(root / input);

    fs::path relative = candidate.lexically_relative(root);

    if (relative.empty() || relative.string().starts_with("..")) {
        throw std::invalid_argument("Path escapes project directory");
    }

    return relative.string();
} 
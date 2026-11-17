#include "RepositoryPaths.hpp"
#include <stdexcept>

namespace minigit {

RepositoryPaths::RepositoryPaths(const std::filesystem::path& root)
    : root_(std::filesystem::weakly_canonical(root)) {
}

std::string RepositoryPaths::safeRelativeFile(const std::string& argument) const {
    if (argument.empty()) {
        throw std::invalid_argument("File path cannot be empty");
    }

    std::filesystem::path input(argument);

    if (input.is_absolute()) {
        throw std::invalid_argument("Absolute paths are not allowed");
    }
    auto result = (root_ / input).lexically_normal();

    auto relative = result.lexically_relative(root_);

    if (relative.empty() || *relative.begin() == "..") {
        throw std::invalid_argument("Path escapes project directory");
    }
    return relative.generic_string();
}

}


#include "RepositoryPaths.hpp"
#include <filesystem>
#include <stdexcept>

namespace minigit {

RepositoryPaths::RepositoryPaths(const std::filesystem::path& root)
    : rootPath(root) {
}

const std::filesystem::path& RepositoryPaths::projectRoot() const {
    return rootPath;
}

std::filesystem::path RepositoryPaths::minigitDirectory() const{
    return projectRoot() / ".minigit";
}

const std::filesystem::path RepositoryPaths::headFile() const {
   return minigitDirectory() / "HEAD";
}
  
std::filesystem::path RepositoryPaths::indexFile() const {
    return minigitDirectory() / "index";
}
  
std::filesystem::path RepositoryPaths::commitsDirectory() const {
    return minigitDirectory() / "commits";
}

std::filesystem::path RepositoryPaths::stageDirectory() const{
    return minigitDirectory() / "Stage";
}

std::filesystem::path RepositoryPaths::stageFile(const std::string& path) const {
    return stageDirectory() / safeRelativeFile(path);
}
  
std::string RepositoryPaths::safeRelativeFile(const std::string& argument) const {
    if (argument.empty()) {
        throw std::invalid_argument("File path cannot be empty");
    }

    std::filesystem::path input(argument);

    if (input.is_absolute() || input.has_root_path()) {
        throw std::invalid_argument("Absolute paths are not allowed");
    }

    auto rootPath = RepositoryPaths::projectRoot();
    auto result = (rootPath / input).lexically_normal();
    auto relative = result.lexically_relative(rootPath);

    if (relative.empty() || *relative.begin() == "..") {
        throw std::invalid_argument("Path escapes project directory");
    }
    return relative.generic_string();
}

} // namespace minigit


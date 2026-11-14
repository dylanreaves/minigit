#include "FileSystem.hpp"
#include <algorithm>
#include <stdexcept>

namespace minigit {

std::vector<std::string> FileSystem::listWorkingFiles(const std::filesystem::path& projectRoot) const {
    namespace fs = std::filesystem;

    // Invalid project root should report an error
    if (!fs::is_directory(projectRoot)) {
        throw std::runtime_error("Cannot access project root: " + projectRoot.string());
    }

    std::vector<std::string> files;

    // Visit all files and folders inside a project root
    // File system iterator handles nested directories for us
    for (fs::recursive_directory_iterator it(projectRoot), end; it != end; ++it) {
        
        // Ignore '.minigit' directories 
        if (it->is_directory() && it->path().filename() == ".minigit") {
            it.disable_recursion_pending();
            continue;
        }

        // Add regular files into the vector
        if (it->is_regular_file()) {
            files.push_back(it->path().lexically_relative(projectRoot).generic_string());
        }
    }

    // Sorting makes the output more predictable and easier to test
    std::sort(files.begin(), files.end());
    return files;
}

} // namespace minigit

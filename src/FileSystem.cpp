#include "FileSystem.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>
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

bool FileSystem::exists(const std::filesystem::path& path) const {
    return std::filesystem::exists(path);
}

std::string FileSystem::readBytes(const std::filesystem::path& path) const {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot read file: " + path.string());
    }

    std::istreambuf_iterator<char> begin(input);
    std::istreambuf_iterator<char> end;
    std::string contents(begin, end);

    if (input.bad()) {
        throw std::runtime_error("Cannot read file: " + path.string());
    }

    return contents;
}

void FileSystem::remove(const std::filesystem::path& path) const {
    namespace fs = std::filesystem;
    std::error_code ec;

    // symlink_status: don't follow links, so a link is removed, never its target.
    const fs::file_status status = fs::symlink_status(path, ec);

    if (ec || !fs::exists(status)) {
        throw std::runtime_error("Cannot remove '" + path.string() + "': file does not exist");
    }

    if (fs::is_directory(status)) {
        throw std::runtime_error("Cannot remove '" + path.string() + "': path is a directory");
    }

    // Non-recursive; never remove_all.
    if (!fs::remove(path, ec)) {
        throw std::runtime_error("Cannot remove '" + path.string() + "': " +
                                 (ec ? ec.message() : "file was not removed"));
    }
}

void FileSystem::copy(const std::filesystem::path& source, const std::filesystem::path& target) const {
    namespace fs = std::filesystem;
    std::error_code ec;

    if (!fs::copy_file(source, target, fs::copy_options::none, ec)) {
        throw (std::runtime_error(
            "Cannot copy '" + source.string() + "' to '" + target.string() + "': " +
            (ec ? ec.message() : "copy was not completed")
        ));
    }
}

void FileSystem::createDirectories(const std::filesystem::path& directory) const {
    std::error_code ec;

    // Creates the directory and any missing parent directories.
    std::filesystem::create_directories(directory, ec);

    // Existing directories are valid, but files and failed creations are not.
    if (ec || !std::filesystem::is_directory(directory)) {
        throw std::runtime_error("Cannot create directory '" + directory.string() + "': " +
            (ec ? ec.message() : "path is not a directory")
        );
    }
}

void FileSystem::writeText(const std::filesystem::path& path, const std::string& text) const {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("Cannot write file: " + path.string());
    }

    // Write the supplied contents exactly, including empty strings.
    output.write(text.data(), static_cast<std::streamsize>(text.size()));
    if (!output) {
        throw std::runtime_error("Cannot write file: " + path.string());
    }
    output.close();

    if (!output) {
        throw std::runtime_error("Cannot finish writing file: " + path.string());
    }
}

} // namespace minigit

#pragma once
#include <filesystem>
#include <fstream>
#include <string>

namespace minigit{
    // Week 1 Issues will add small helpers for building paths below a repository root.
    // Path construction describes locations; it does not create files or directories.
    std::filesystem::path metadataDirectory(const std::filesystem::path &root);
    std::filesystem::path getHead(const std::filesystem::path &root);
    bool createparentdirectories(const std::filesystem::path &path);

    bool writeTextFile(const std::filesystem::path& path, const std::string& text);
    // Reads the entire file without changing text or line endings.
    // Returns false if the file cannot be opened or read.
    bool readTextFile(const std::filesystem::path& path, std::string& text);

} // namespace minigit

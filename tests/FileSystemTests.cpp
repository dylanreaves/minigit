
#include "FileSystem.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }

    std::cout << "PASS: " << message << '\n';
    return true;
}

int main() {
    // Create a unique temporary project directory.
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path tempRoot = fs::temp_directory_path() / ("minigit-fs1-" + std::to_string(stamp));

    // Remove temporary test files when the program finishes.
    struct Cleanup {
        fs::path path;
        ~Cleanup() { std::error_code ec; fs::remove_all(path, ec); }
    } cleanup{tempRoot};

    // Create test directories.
    fs::create_directories(tempRoot / "docs");
    fs::create_directories(tempRoot / ".minigit" / "objects");
    fs::create_directories(tempRoot / "empty");

    // Create test files.
    {
        std::ofstream(tempRoot / "notes.txt") << "original";
        std::ofstream(tempRoot / "docs" / "guide.txt") << "nested";
        std::ofstream(tempRoot / ".minigit" / "objects" / "hidden.txt") << "metadata";
    }

    const minigit::FileSystem fileSystem;

    // Test 1: Ordinary files, nested files, and .minigit exclusion.
    const auto files = fileSystem.listWorkingFiles(tempRoot);

    const std::vector<std::string> expected = {
        "docs/guide.txt",
        "notes.txt"
    };

    if (!check(files == expected, 
        "lists ordinary and nested files, excluding directories and .minigit")) return 1;

    // Test 2: Empty project directory.
    if (!check(fileSystem.listWorkingFiles(tempRoot / "empty").empty(), 
        "returns empty list for empty directory")) return 1;

    // Test 3: Missing project root.
    bool missingRootReported = false;

    try {
        fileSystem.listWorkingFiles(tempRoot / "missing");
    } catch (const std::exception&) {
        missingRootReported = true;
    }

    if (!check(missingRootReported, "reports missing project root")) return 1;

    // Test 4: Listing does not change existing files.
    std::string original;

    {
        std::ifstream input(tempRoot / "notes.txt");
        std::getline(input, original);
    }

    if (!check(original == "original" &&
        fs::exists(tempRoot / "docs" / "guide.txt"),
        "listing does not modify working files")) return 1;

    return 0;
}

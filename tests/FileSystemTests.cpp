
#include "FileSystem.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
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

    // Test 5: Existing file and directory paths.
    if (!check(fileSystem.exists(tempRoot / "notes.txt"),
        "returns true for an existing file")) return 1;
    if (!check(fileSystem.exists(tempRoot / "docs"),
        "returns true for an existing directory")) return 1;

    // Test 6: Nonexistent file and directory paths.
    const fs::path missingFile = tempRoot / "missing.txt";
    const fs::path missingDirectory = tempRoot / "missing-directory";
    if (!check(!fileSystem.exists(missingFile),
        "returns false for a nonexistent file")) return 1;
    if (!check(!fileSystem.exists(missingDirectory),
        "returns false for a nonexistent directory")) return 1;

    // Test 7: Calling exists does not create or modify paths.
    const fs::path unchangedFile = tempRoot / "notes.txt";
    std::string before;
    {
        std::ifstream input(unchangedFile);
        std::getline(input, before);
    }

    (void)fileSystem.exists(unchangedFile);
    (void)fileSystem.exists(missingFile);
    (void)fileSystem.exists(missingDirectory);

    std::string after;
    {
        std::ifstream input(unchangedFile);
        std::getline(input, after);
    }

    if (!check(after == before &&
        !fs::exists(missingFile) &&
        !fs::exists(missingDirectory),
        "exists does not create or modify files or directories")) return 1;

    // Test 8: Read text and multiline file contents exactly.
    if (!check(fileSystem.readBytes(tempRoot / "notes.txt") == "original",
        "reads an existing text file")) return 1;

    const fs::path multilineFile = tempRoot / "multiline.txt";
    {
        std::ofstream output(multilineFile, std::ios::binary);
        output << "first\nsecond\r\nthird\n";
    }
    if (!check(fileSystem.readBytes(multilineFile) == "first\nsecond\r\nthird\n",
        "preserves newline characters in multiline files")) return 1;

    // Test 9: Preserve binary bytes and read empty files.
    const fs::path binaryFile = tempRoot / "binary.dat";
    const std::string binaryContents = {'A', '\0', 'B', '\0', '\x7f'};
    {
        std::ofstream output(binaryFile, std::ios::binary);
        output.write(binaryContents.data(), static_cast<std::streamsize>(binaryContents.size()));
    }
    if (!check(fileSystem.readBytes(binaryFile) == binaryContents,
        "preserves binary bytes including null bytes")) return 1;

    const fs::path emptyFile = tempRoot / "empty.txt";
    {
        std::ofstream output(emptyFile, std::ios::binary);
    }
    if (!check(fileSystem.readBytes(emptyFile).empty(),
        "returns an empty string for an empty file")) return 1;

    // Test 10: Missing files report an error.
    bool missingFileReported = false;
    try {
        (void)fileSystem.readBytes(missingFile);
    } catch (const std::exception&) {
        missingFileReported = true;
    }
    if (!check(missingFileReported,
        "reports an error for a nonexistent file")) return 1;

    // Test 11: Reading does not modify the original file.
    std::string fileBefore;
    {
        std::ifstream input(binaryFile, std::ios::binary);
        fileBefore.assign(
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());
    }
    (void)fileSystem.readBytes(binaryFile);
    std::string fileAfter;
    {
        std::ifstream input(binaryFile, std::ios::binary);
        fileAfter.assign(
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());
    }
    if (!check(fileBefore == fileAfter,
        "reading does not modify the original file")) return 1;

    return 0;
}

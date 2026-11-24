
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



            // ---- remove() tests ----
    const fs::path removeRoot = tempRoot / "remove-tests";
    fs::create_directories(removeRoot / "folder");

    const fs::path target = removeRoot / "target.txt";
    const fs::path neighborA = removeRoot / "neighborA.txt";
    const fs::path neighborB = removeRoot / "neighborB.txt";
    const fs::path folderChild = removeRoot / "folder" / "child.txt";
    {
        std::ofstream(target) << "delete me";
        std::ofstream(neighborA) << "keep A";
        std::ofstream(neighborB) << "keep B";
        std::ofstream(folderChild) << "keep child";
    }

    auto readAll = [](const fs::path& p) {
        std::ifstream input(p, std::ios::binary);
        return std::string(std::istreambuf_iterator<char>(input),
                           std::istreambuf_iterator<char>());
    };

    // Remove test 1: Removes an existing file.
    fileSystem.remove(target);
    if (!check(!fs::exists(target),
        "removes an existing file")) return 1;

    // Remove test 2: Neighboring files are unchanged.
    if (!check(fs::exists(neighborA) && readAll(neighborA) == "keep A" &&
               fs::exists(neighborB) && readAll(neighborB) == "keep B",
        "neighboring files remain unchanged")) return 1;

    // Remove test 3: Nonexistent file reports an error.
    bool missingRemoveReported = false;
    try {
        fileSystem.remove(removeRoot / "does-not-exist.txt");
    } catch (const std::exception&) {
        missingRemoveReported = true;
    }
    if (!check(missingRemoveReported,
        "reports an error when removing a nonexistent file")) return 1;

    // Remove test 4: Nonempty directory is not recursively deleted.
    bool directoryRemoveReported = false;
    try {
        fileSystem.remove(removeRoot / "folder");
    } catch (const std::exception&) {
        directoryRemoveReported = true;
    }
    if (!check(directoryRemoveReported &&
               fs::is_directory(removeRoot / "folder") &&
               readAll(folderChild) == "keep child",
        "does not recursively delete a nonempty directory")) return 1;


    // Copy test 1: Copy an ordinary text file.
    const fs::path copyRoot = tempRoot / "copy-tests";
    fs::create_directories(copyRoot);
    const fs::path textCopy = copyRoot / "notes-copy.txt";
    fileSystem.copy(tempRoot / "notes.txt", textCopy);
    if (!check(fileSystem.readBytes(textCopy) == "original", "copies an ordinary text file")) return 1;

    // Copy test 2: Preserve multiline file contents.
    const fs::path multilineCopy = copyRoot / "multiline-copy.txt";
    fileSystem.copy(multilineFile, multilineCopy);

    if (!check(fileSystem.readBytes(multilineCopy) == "first\nsecond\r\nthird\n",
        "preserves exact multiline contents")) return 1;

    // Copy test 3: Preserve binary bytes.
    const fs::path binaryCopy = copyRoot / "binary-copy.dat";
    fileSystem.copy(binaryFile, binaryCopy);
    if (!check(fileSystem.readBytes(binaryCopy) == binaryContents, "preserves binary bytes")) return 1;

    // Copy test 4: Missing source reports an error.
    bool missingCopyReported = false;
    try {
        fileSystem.copy(copyRoot / "missing.txt", copyRoot / "output.txt");
    } catch (const std::exception&) {
        missingCopyReported = true;
    }
    if (!check(missingCopyReported, "reports error for missing copy source")) return 1;

    // Copy test 5: Invalid destination reports an error.
    bool invalidTargetReported = false;
    try {
        fileSystem.copy(tempRoot / "notes.txt", copyRoot / "missing-folder" / "output.txt");
    } catch (const std::exception&) {
        invalidTargetReported = true;
    }
    if (!check(invalidTargetReported, "reports error for invalid copy destination")) return 1;

    // Copy test 6: Source and unrelated files remain unchanged.
    if (!check(
        fileSystem.readBytes(tempRoot / "notes.txt") == "original" &&
        fileSystem.readBytes(binaryFile) == binaryContents &&
        fileSystem.readBytes(tempRoot / "docs" / "guide.txt") == "nested",
        "copy preserves source and unrelated files")) return 1;

    // Copy test 7: Existing destination must not be overwritten.
    const fs::path existingTarget = copyRoot / "existing.txt";
    { std::ofstream(existingTarget) << "keep this"; }

    bool existingTargetReported = false;
    try {
        fileSystem.copy(tempRoot / "notes.txt", existingTarget);
    } catch (const std::exception&) {
        existingTargetReported = true;
    }
    if (!check(existingTargetReported && fileSystem.readBytes(existingTarget) == "keep this",
        "rejects existing destination without overwriting it")) return 1;

    // Copy test 8: Copy an empty file.
    const fs::path emptyCopy = copyRoot / "empty-copy.txt";
    fileSystem.copy(emptyFile, emptyCopy);
    if (!check(fs::exists(emptyCopy) && fileSystem.readBytes(emptyCopy).empty(),
        "copies an empty file successfully")) return 1;


    // ---- INIT-2: createDirectories() and writeText() tests ----
    const fs::path initRoot = tempRoot / "init-2-tests";

    // Test 1: Create a new directory.
    fileSystem.createDirectories(initRoot);
    if (!check(fs::is_directory(initRoot), "creates a new directory")) return 1;

    // Test 2: Create nested directories.
    const fs::path nestedDir = initRoot / "nested" / "subfolder";
    fileSystem.createDirectories(nestedDir);

    if (!check(fs::is_directory(nestedDir), "creates nested directories")) return 1;

    // Test 3: Existing directory must preserve its contents.
    const fs::path existingFile = initRoot / "existing.txt";
    { std::ofstream(existingFile) << "keep this"; }
    fileSystem.createDirectories(initRoot);
    if (!check(fileSystem.readBytes(existingFile) == "keep this", "preserves contents of an existing directory")) return 1;

    // Test 4: Existing regular file cannot be used as a directory.
    bool fileAsDirectoryRejected = false;
    try {
        fileSystem.createDirectories(existingFile);
    } catch (const std::exception&) {
        fileAsDirectoryRejected = true;
    }
    if (!check(fileAsDirectoryRejected, "rejects regular file as directory")) return 1;

    // Test 5: Write exact text contents.
    const fs::path textFile = initRoot / "text.txt";
    const std::string exactText = "First line\nSecond line\r\n";
    fileSystem.writeText(textFile, exactText);
    if (!check(fileSystem.readBytes(textFile) == exactText, "writes exact text contents")) return 1;

    // Test 6: Create an empty file.
    const fs::path emptyTextFile = initRoot / "empty.txt";
    fileSystem.writeText(emptyTextFile, "");

    if (!check(fs::exists(emptyTextFile) && fileSystem.readBytes(emptyTextFile).empty(), "creates an empty file")) return 1;

    // Test 7: Overwrite an existing file.
    fileSystem.writeText(textFile, "updated");
    if (!check(fileSystem.readBytes(textFile) == "updated", "overwrites existing file contents")) return 1;

    // Test 8: Missing parent directory must cause an error.
    bool missingParentRejected = false;
    const fs::path invalidFile = initRoot / "missing-parent" / "file.txt";
    try {
        fileSystem.writeText(invalidFile, "test");
    } catch (const std::exception&) {
        missingParentRejected = true;
    }

    if (!check(missingParentRejected && !fs::exists(invalidFile) && !fs::exists(invalidFile.parent_path()),
        "rejects writes when parent directory does not exist")) return 1;

    // Test 9: A directory cannot be used as a text file.
    bool directoryTargetRejected = false;
    try {
        fileSystem.writeText(initRoot, "test");
    } catch (const std::exception&) {
        directoryTargetRejected = true;
    }

    if (!check(directoryTargetRejected, "rejects directory as file destination")) return 1;

    // Test 10: Unrelated files remain unchanged.
    if (!check(fileSystem.readBytes(existingFile) == "keep this" &&
            fileSystem.readBytes(tempRoot / "notes.txt") == "original",
        "leaves unrelated files unchanged")) return 1;

    return 0;
}

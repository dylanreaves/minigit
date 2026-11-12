#include "PathLab.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

int main() {
    namespace fs = std::filesystem;
    const fs::path root = "course/repo";
    if (!check(fs::path("course/repo").filename() == "repo", "path smoke check")) return 1;
    if (!check(minigit::metadataDirectory(root) == root / ".minigit", "metadata directory")) return 1;
    if (!check(minigit::getHead(root) == root / ".minigit" / "HEAD", "HEAD path")) return 1;
    const fs::path nestedRoot = "course/repo/lab12";
    if (!check(minigit::getHead(nestedRoot) == nestedRoot / ".minigit" / "HEAD", "nested HEAD path")) return 1;

    // Temporary test directory is removed even when a check fails.
    const fs::path tempDir = fs::temp_directory_path() / "minigit_issue3_read_test";
    struct Cleanup {
        fs::path path;
        ~Cleanup() { std::error_code ec; fs::remove_all(path, ec); }
    } cleanup{tempDir};

    std::error_code ec;
    fs::create_directories(tempDir, ec);
    if (!check(!ec, "create temporary directory")) return 1;

    const fs::path testFile = tempDir / "sample.txt";
    const std::string expected = "First line\nSecond line\n";
    {
        std::ofstream output(testFile, std::ios::binary);
        if (!check(output.is_open(), "create test file")) return 1;
        output.write(expected.data(), static_cast<std::streamsize>(expected.size()));
        if (!check(output.good(), "write test contents")) return 1;
    }

    std::string actual;
    if (!check(minigit::readTextFile(testFile, actual), "read existing file")) return 1;
    if (!check(actual == expected, "preserve exact contents and newlines")) return 1;

    actual = "unchanged";
    if (!check(!minigit::readTextFile(tempDir / "missing.txt", actual), "missing file returns false")) return 1;
    if (!check(actual == "unchanged", "failed read leaves output unchanged")) return 1;

    std::cout << "All PathLab tests passed\n";

    {
        // Issue 3 test:
        const std::filesystem::path testFile =
        "build/issue3-test/nested/output.txt";

        if (!check(minigit::writeTextFile(testFile, "Hello MiniGit"),
           "writeTextFile should create parent directories and write text")) {
            return 1;
        }

            std::ifstream input(testFile);
            std::string contents;
            std::getline(input, contents);

        if (!check(contents == "Hello MiniGit",
           "writeTextFile should save the exact text")) {
            return 1;
    }
    input.close();
    std::filesystem::remove_all("build/issue3-test");


    std::cout << "PathLab smoke check passed\n";
    }
    return 0;
}

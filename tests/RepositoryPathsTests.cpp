
#include "RepositoryPaths.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <stdexcept>

namespace fs = std::filesystem;

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }

    std::cout << "PASS: " << message << '\n';
    return true;
}

bool headExists(const std::string& root = ".minigit"){
        fs::path headPath = fs::path(root) / "HEAD";
        return fs::exists(headPath) && is_regular_file(headPath);
}

int main() {
    // Test 1: Ordinary repository root
    const fs::path root = "course/repo";
    const minigit::RepositoryPaths paths(root);
    if (!check(paths.projectRoot() == root, "ordinary repository root")) return 1;

    // Test 2: Nested repository root
    const fs::path nestedRoot = "course/team/repo";
    const minigit::RepositoryPaths nested(nestedRoot);
    if (!check(nested.projectRoot() == nestedRoot, "nested repository root")) return 1;

    // Test 3: Getter does not create a missing directory
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path missingRoot = fs::temp_directory_path() / ("minigit-rp1-" + std::to_string(stamp));
    if (!check(!fs::exists(missingRoot), "test directory initially absent")) return 1;

    const minigit::RepositoryPaths missing(missingRoot);
    if (!check(missing.projectRoot() == missingRoot, "returns nonexistent root path")) return 1;
    if (!check(!fs::exists(missingRoot), "does not create directories")) return 1;

    // Test 4: Getter leaves stored root unchanged
    const fs::path original = paths.projectRoot();
    if (!check(paths.projectRoot() == original, "stored root remains unchanged")) return 1;

    // Test 5: Check that valid relative paths are accepted
    if (!check(paths.safeRelativeFile("docs/readme.txt") == "docs/readme.txt", "valid relative path")) return 1;

    // Test 6: Check that paths cannot escape the project root.
    bool rejected = false;
    try {
        paths.safeRelativeFile("../outside.txt");
    } catch(const std::invalid_argument) {
        rejected = true;
    }
    if (!check(rejected, "reject path outside of repository")) return 1;

    // Test 7: Verify that headFile() returns the correct path for an ordinary root.
    if (!check(paths.headFile() == root/".minigit"/"HEAD", "ordinary_root.minigit/HEAD"));
    
    // Test 8: Verify that headFile() works with a nested root.
    if (!check(paths.headFile() == nestedRoot/".minigit"/"HEAD", "nest_ordinary_root.minigit/HEAD"));

    // Test 9: Test when the HEAD file does not exist and confirm the function only returns its path without creating it.
    //check the head not exist, and head file return still HEAD which not exist
    if (!check((headExists() == false && paths.headFile() == root/".minigit"/"HEAD"), "ordinary_root.minigit path, head not exist"));

    return 0;
}


#include "RepositoryPaths.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

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

    return 0;
}

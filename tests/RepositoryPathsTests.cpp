
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
    /**************** MILESTONE 1***************************/

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
    
    // Test 7: indexFile returns the correct path
    if (!check(paths.indexFile() == root / ".minigit" / "index",
           "index file path for ordinary root")) return 1;

    if (!check(nested.indexFile() == nestedRoot / ".minigit" / "index",
           "index file path for nested root")) return 1;

    // Test 8: indexFile does not create the index file
    if (!check(!fs::exists(paths.indexFile()),
           "index file is not created by getter")) return 1;

        // Test 7: Reject empty relative path
    rejected = false;
    try {
        paths.safeRelativeFile("");
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    if (!check(rejected, "reject empty relative path")) return 1;

    // Test 8: Reject absolute path
    rejected = false;
    try {
        paths.safeRelativeFile("/outside.txt");
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    if (!check(rejected, "reject absolute path")) return 1;

    // Test 7: Verify that headFile() returns the correct path for an ordinary root.
    if (!check(paths.headFile() == root/".minigit"/"HEAD", "ordinary_root.minigit/HEAD"));
    
    // Test 8: Verify that headFile() works with a nested root.
    if (!check(paths.headFile() == nestedRoot/".minigit"/"HEAD", "nest_ordinary_root.minigit/HEAD"));

    // Test 9: Test when the HEAD file does not exist and confirm the function only returns its path without creating it.
    if (!check((headExists() == false && paths.headFile() == root/".minigit"/"HEAD"), "ordinary_root.minigit path, head not exist"));

    // ---- RP-6: stageFile() tests ----

    // Test 1: Ordinary file path.
    if (!check(paths.stageFile("notes.txt") == root / ".minigit" / "Stage" / "notes.txt",
        "stageFile returns correct path for ordinary file"
    )) return 1;

    // Test 2: Nested file path.
    if (!check(paths.stageFile("docs/guide.txt") == root / ".minigit" / "Stage" / "docs" / "guide.txt",
        "stageFile preserves nested file paths"
    )) return 1;

    // Test 3: Nested project root.
    if (!check(nested.stageFile("notes.txt") == nestedRoot / ".minigit" / "Stage" / "notes.txt",
        "stageFile works with nested project root"
    )) return 1;

    // Test 4: Reject paths escaping the project.
    bool invalidStagePathRejected = false;
    try {
        paths.stageFile("../outside.txt");
    } catch (const std::invalid_argument&) {
        invalidStagePathRejected = true;
    }
    if (!check(invalidStagePathRejected, "stageFile rejects paths outside project")) return 1;

    // Test 5: Calling stageFile does not create files.
    const fs::path expectedStageFile = missingRoot / ".minigit" / "Stage" / "notes.txt";

    if (!check(missing.stageFile("notes.txt") == expectedStageFile && !fs::exists(missingRoot),
        "stageFile returns path without creating directories or files"
    )) return 1;

    /**************** MILESTONE 2***************************/

    /*********Test Initialization Directory Path**************/
    //Normal project root produces the correct .minigit path.
    if (!check(paths.minigitDirectory() == root / ".minigit", "ordinary repository root/.minigit path"));

    // Normal project root produces the correct Stage path.
    if (!check(paths.stageDirectory() == root / ".minigit", "ordinary repository root/.minigit path"));

    // Nested project root produces both correct paths.
    if (!check(paths.minigitDirectory() == nestedRoot / ".minigit", "nest_ordinary_root/.minigit path"));
    if (!check(paths.stageDirectory() == nestedRoot / "Stage", "nest_ordinary_root/ Stage path"));

    // Calling either function does not create any directories.
    if (!check(!fs::exists(paths.minigitDirectory()),
           "minigitDirectory does not create any directories")) return 1;
    if (!check(!fs::exists(paths.stageDirectory()),
           "stageDirectory does not create any directories")) return 1;

    return 0;
}

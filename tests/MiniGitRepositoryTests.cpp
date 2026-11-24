#include "MiniGitRepository.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>

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
    // Create a temporary project directory.
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path tempRoot = fs::temp_directory_path() / ("minigit-repository-" + std::to_string(stamp));

    // Remove the temporary directory after testing.
    struct Cleanup {
        fs::path path;

        ~Cleanup() {
            std::error_code ec;
            fs::remove_all(path, ec);
        }
    } cleanup{tempRoot};

    fs::create_directories(tempRoot);

    // Create the supporting objects.
    minigit::RepositoryPaths paths(tempRoot);
    minigit::FileSystem fileSystem;

    // Test 1: Repository is not initialized before construction.
    if (!check(!fileSystem.exists(tempRoot / ".minigit"), "repository initially has no metadata directory")) {
        return 1;
    }

    // Test 2: Construct MiniGitRepository.
    minigit::MiniGitRepository repository(paths, fileSystem);
    (void)repository;

    if (!check(true, "MiniGitRepository constructs successfully")) {
        return 1;
    }

    // Test 3: Construction does not initialize the repository.
    if (!check(!fileSystem.exists(tempRoot / ".minigit"), "constructor does not create .minigit")) {
        return 1;
    }

    // Test 4: The original project root remains unchanged.
    if (!check(paths.projectRoot() == tempRoot, "constructor does not modify RepositoryPaths")) {
        return 1;
    }

    return 0;
}
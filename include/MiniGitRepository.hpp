#pragma once
#include "RepositoryPaths.hpp"
#include "FileSystem.hpp"

namespace minigit {

class MiniGitRepository {
    private:
        RepositoryPaths& paths;
        FileSystem& fileSystem;
    public:
        MiniGitRepository(RepositoryPaths& paths, FileSystem& fileSystem);
        void init();
};

} // namespace minigit
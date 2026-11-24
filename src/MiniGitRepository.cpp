#include "MiniGitRepository.hpp"

namespace minigit {

MiniGitRepository::MiniGitRepository(RepositoryPaths& paths,FileSystem& fileSystem)
    : paths(paths), fileSystem(fileSystem) {
}

} // namespace minigit
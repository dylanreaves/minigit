cmake_minimum_required(VERSION 3.20)
project(MiniGitWeek1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(minigit_paths 
    src/PathLab.cpp
    src/RepositoryPaths.cpp
    src/FileSystem.cpp
    src/MiniGitRepository.cpp
)

target_include_directories(minigit_paths PUBLIC include)

include(CTest)

if(BUILD_TESTING)
    add_executable(path_lab_tests tests/PathLabTests.cpp)
    target_link_libraries(path_lab_tests PRIVATE minigit_paths)
    add_test(NAME path_lab_tests COMMAND path_lab_tests)

    add_executable(repository_paths_tests tests/RepositoryPathsTests.cpp)
    target_link_libraries(repository_paths_tests PRIVATE minigit_paths)
    add_test(NAME repository_paths_tests COMMAND repository_paths_tests)

    add_executable(file_system_tests tests/FileSystemTests.cpp)
    target_link_libraries(file_system_tests PRIVATE minigit_paths)
    add_test(NAME file_system_tests COMMAND file_system_tests)

    add_executable(minigit_repository_tests tests/MiniGitRepositoryTests.cpp)
    target_link_libraries(minigit_repository_tests PRIVATE minigit_paths)
    add_test(NAME minigit_repository_tests COMMAND minigit_repository_tests)
endif()

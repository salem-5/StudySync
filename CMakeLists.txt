cmake_minimum_required(VERSION 3.16)
project(StudySync VERSION 1.2.5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tests)

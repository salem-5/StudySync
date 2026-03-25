cmake_minimum_required(VERSION 3.21)
project(StudyPlanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES
        "src/*.cpp"
        "include/*.h"
)

add_executable(StudyPlanner ${PROJECT_SOURCES})

target_include_directories(StudyPlanner PRIVATE include src)
target_include_directories(StudyPlanner PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(StudyPlanner PRIVATE Qt6::Widgets)
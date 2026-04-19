find_package(Boost REQUIRED)

add_library(studysync_shared STATIC
        src/DataStructures.cpp
        src/TcpClient.cpp
)

target_include_directories(studysync_shared PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(studysync_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
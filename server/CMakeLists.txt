find_package(Boost REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
list(APPEND SRC_FILES src/sqlite/sqlite3.c)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_executable(studysync_server
        ${SRC_FILES}
        ${HEADER_FILES}
)

target_link_libraries(studysync_server PRIVATE studysync_shared)

target_include_directories(studysync_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(studysync_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/sqlite)
target_include_directories(studysync_server PRIVATE ${Boost_INCLUDE_DIRS})
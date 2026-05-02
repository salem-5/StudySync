# Find dependencies
find_package(GTest REQUIRED CONFIG COMPONENTS gtest gmock)
find_package(Boost REQUIRED)

file(GLOB_RECURSE SERVER_SRC_FILES "${CMAKE_SOURCE_DIR}/server/src/*.cpp")
list(APPEND SERVER_SRC_FILES "${CMAKE_SOURCE_DIR}/server/src/sqlite/sqlite3.c")
list(REMOVE_ITEM SERVER_SRC_FILES "${CMAKE_SOURCE_DIR}/server/src/main.cpp")
file(GLOB_RECURSE TEST_FILES "*.cpp")
add_executable(studysync_tests
        ${SERVER_SRC_FILES}
        ${TEST_FILES}
)

target_link_libraries(studysync_tests PRIVATE
        studysync_shared
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

target_include_directories(studysync_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/server/include
        ${CMAKE_SOURCE_DIR}/server/include/sqlite
        ${Boost_INCLUDE_DIRS}
)

include(GoogleTest)
gtest_discover_tests(studysync_tests)
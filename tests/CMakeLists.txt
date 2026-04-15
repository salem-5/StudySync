add_executable(test_studysync test_studysync.cpp)
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

target_link_libraries(test_studysync
    gtest
    gtest_main
    pthread
)

add_test(NAME StudySyncTests COMMAND test_studysync)

add_executable(test_studysync test_studysync.cpp)

target_link_libraries(test_studysync
    gtest
    gtest_main
    pthread
)

add_test(NAME StudySyncTests COMMAND test_studysync)

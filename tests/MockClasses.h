#pragma once
#include <gmock/gmock.h>
#include "TcpServer.h"
#include "Database.h"
#include "ConnectionManager.h"

class MockTcpConnection : public TcpConnection {
public:
    MockTcpConnection(boost::asio::io_context& io_context) : TcpConnection(io_context) {}

    MOCK_METHOD(void, send, (const std::string& message), ());
    MOCK_METHOD(void, start, (), ());
};

class MockDatabase {
public:
    MOCK_METHOD(int, createUser, (const std::string&, const std::string&, const std::string&));
    MOCK_METHOD(bool, validateLogin, (const std::string&, const std::string&, int&));
    MOCK_METHOD(LoginPayload, getFullUserData, (int, const std::string&));
    MOCK_METHOD(int, getSyncCounter, ());
    MOCK_METHOD(void, createTemplateForUser, (int));
    MOCK_METHOD(std::vector<int>, getUsersInGroup, (int));
};
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include "MockClasses.h"
#include "MessageRouter.h"

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgReferee;

extern void registerServerRoutes(int port);

class ServerRoutesTest : public ::testing::Test {
protected:
    boost::asio::io_context io_context;
    std::shared_ptr<MockTcpConnection> mockConn;

    void SetUp() override {
        mockConn = std::make_shared<MockTcpConnection>(io_context);
        registerServerRoutes(8080);
    }
};

TEST_F(ServerRoutesTest, PingRoute) {
    boost::json::object req = {
        {"req_id", 1},
        {"cmd", "ping"}
    };
    boost::json::object res;

    bool handled = MessageRouter::getInstance().handle("ping", req, res);

    EXPECT_TRUE(handled);
    EXPECT_EQ(res.at("status").as_string(), "success");
}

TEST_F(ServerRoutesTest, CreateUserFormatValidation) {
    boost::json::object req = {
        {"req_id", 2},
        {"cmd", "createUser"},
        {"username", "newUser"},
        {"email", "invalid-email-format"},
        {"password", "pass123"}
    };
    boost::json::object res;

    bool handled = MessageRouter::getInstance().handle("createUser", req, res);

    EXPECT_TRUE(handled);
    EXPECT_EQ(res.at("status").as_string(), "error");
    EXPECT_EQ(res.at("message").as_string(), "Invalid email format.");
}

TEST_F(ServerRoutesTest, TcpConnectionHandleValidMessage) {
    std::string jsonStr = R"({"req_id": 99, "cmd": "ping"})";
    EXPECT_CALL(*mockConn, send(::testing::HasSubstr("\"status\":\"success\"")))
        .Times(1);

    mockConn->handleMessage(jsonStr);
}
#include <gtest/gtest.h>
#include <boost/json.hpp>
#include "MessageRouter.h"

class MessageRouterTest : public ::testing::Test {
protected:
    void SetUp() override {
        MessageRouter::getInstance().clearRoutes();
    }
};

TEST_F(MessageRouterTest, RegisterAndHandleValidRoute) {
    MessageRouter& router = MessageRouter::getInstance();

    bool handlerCalled = false;
    router.registerRoute("test_cmd", [&](const boost::json::object& req, boost::json::object& res) {
        handlerCalled = true;
        res["status"] = "success";
        res["echo"] = req.at("data");
    });

    boost::json::object req = {{"cmd", "test_cmd"}, {"data", "hello"}};
    boost::json::object res;

    bool result = router.handle("test_cmd", req, res);

    EXPECT_TRUE(result);
    EXPECT_TRUE(handlerCalled);
    EXPECT_EQ(res.at("status").as_string(), "success");
    EXPECT_EQ(res.at("echo").as_string(), "hello");
}

TEST_F(MessageRouterTest, HandleInvalidRoute) {
    MessageRouter& router = MessageRouter::getInstance();

    boost::json::object req = {{"cmd", "unknown_cmd"}};
    boost::json::object res;

    bool result = router.handle("unknown_cmd", req, res);

    EXPECT_FALSE(result);
}
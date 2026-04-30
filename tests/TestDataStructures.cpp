#include <gtest/gtest.h>
#include <boost/json.hpp>
#include "DataStructures.h"

TEST(DataStructuresTest, AiMessageGettersSetters) {
    std::vector<int> attachments = {1, 2, 3};
    AiMessage msg("user", "Hello AI", attachments);

    EXPECT_EQ(msg.getRole(), "user");
    EXPECT_EQ(msg.getText(), "Hello AI");
    EXPECT_EQ(msg.getAttachments().size(), 3);

    msg.setRole("ai");
    msg.setText("Hi User");
    EXPECT_EQ(msg.getRole(), "ai");
    EXPECT_EQ(msg.getText(), "Hi User");
}

TEST(DataStructuresTest, UserJsonSerialization) {
    User user(1, "testuser", "test@email.com", "password123", true);
    user.addGroupId(10);
    user.addPinnedGroupId(20);

    boost::json::object jsonObj = user.toJson();

    EXPECT_EQ(jsonObj.at("id").as_int64(), 1);
    EXPECT_EQ(jsonObj.at("username").as_string(), "testuser");
    EXPECT_EQ(jsonObj.at("password").as_string(), "password123");
    EXPECT_EQ(jsonObj.at("groupIds").as_array().size(), 1);

    boost::json::object safeJson = user.toSafeJson();
    EXPECT_FALSE(safeJson.contains("password"));
    EXPECT_TRUE(safeJson.contains("username"));
}

TEST(DataStructuresTest, UserJsonDeserialization) {
    boost::json::object jsonObj = {
        {"id", 2},
        {"username", "alice"},
        {"email", "alice@test.com"},
        {"password", "pass"},
        {"aiAccess", false},
        {"groupIds", boost::json::array{100}},
        {"pinnedGroupIds", boost::json::array{200}}
    };

    User user = User::fromJson(jsonObj);
    EXPECT_EQ(user.getId(), 2);
    EXPECT_EQ(user.getUsername(), "alice");
    EXPECT_FALSE(user.getAiAcess());
    EXPECT_EQ(user.getGroupIds()[0], 100);
}

TEST(DataStructuresTest, TaskJsonSerialization) {
    Task task(1, "Math Homework", "School", false, 10, 11, 50);
    boost::json::object jsonObj = task.toJson();

    EXPECT_EQ(jsonObj.at("id").as_int64(), 1);
    EXPECT_EQ(jsonObj.at("title").as_string(), "Math Homework");
    EXPECT_EQ(jsonObj.at("isCompleted").as_bool(), false);
}

TEST(DataStructuresTest, StudyGroupManagement) {
    StudyGroup group(1, "CS101");
    group.addMemberId(10);
    group.addMemberId(20);
    EXPECT_EQ(group.getMemberIds().size(), 2);

    group.removeMemberId(10);
    EXPECT_EQ(group.getMemberIds().size(), 1);
    EXPECT_EQ(group.getMemberIds()[0], 20);

    Message msg(20, "Hello group!");
    group.addMessage(msg);
    EXPECT_EQ(group.getMessages().size(), 1);
    EXPECT_EQ(group.getMessages()[0].getText(), "Hello group!");
}

TEST(DataStructuresTest, LoginPayloadSerialization) {
    User user(1, "user1", "user1@mail.com", "pass", true);
    std::vector<StudyGroup> groups;
    std::vector<StudyGroup> invites;
    std::vector<Task> tasks;
    std::vector<AiMessage> aiMsgs;

    LoginPayload payload(user, groups, invites, tasks, "token123", 10, aiMsgs);
    boost::json::object jsonObj = payload.toJson();

    EXPECT_EQ(jsonObj.at("sessionToken").as_string(), "token123");
    EXPECT_EQ(jsonObj.at("aiCredits").as_int64(), 10);
    EXPECT_FALSE(jsonObj.at("user").as_object().contains("password"));
}
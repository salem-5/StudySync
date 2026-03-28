#pragma once
#include "DataStructures.h"
#include <vector>
#include <memory>
#include <optional>

class ClientState {
private:
    static std::unique_ptr<User> currentUser;
    static std::vector<StudyGroup> studyGroups;
    static std::vector<Task> tasks;
    static std::string sessionToken;
    static std::unordered_map<int, std::string> usernameCache;

public:
    static const User* getUser();
    static const std::vector<StudyGroup>& getStudyGroups();
    static const std::vector<Task>& getTasks();
    static const StudyGroup* getGroupById(int id);
    static bool isGroupPinned(int groupId);
    static void mockTogglePinGroup(int groupId);
    static std::string getUsername(int userId);
    static void initDummyData();
    static void loadFromPayload(const LoginPayload& payload);

    static void clear();
    static void mockRequestUsername(int userId);
    static void mockCreateGroup(const std::string& groupName);
    static void mockSendMessage(int groupId, const std::string& text);
    static void mockCreateTask(int groupId, const std::string& title, const std::string& category, int assigneeId);
};
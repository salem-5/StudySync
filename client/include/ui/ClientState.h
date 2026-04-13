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
    static std::vector<StudyGroup> pendingInvites;

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
    static int mockGetUserIdByUsername(const std::string& username);
    static void mockToggleTaskCompletion(int taskId, bool completed);
    static void mockEditTask(int taskId, const std::string& title, const std::string& tag, int assigneeId);
    static void mockAddMemberToGroup(int groupId, const std::string& username);
    static void mockRemoveMemberFromGroup(int groupId, int userId);
    static void mockDeleteGroup(int groupId);
    static void mockDeleteTask(int taskId);
    static void mockInviteMemberToGroup(int groupId, const std::string& username);
    static void mockCancelInvite(int groupId, int userId);
    static void mockAcceptInvite(int groupId, int userId);
    static void mockDenyInvite(int groupId, int userId);
    static void mockCreateUser(const std::string& username, const std::string& email, const std::string& password);
    static void mockDeleteUser(int userId);
    static std::vector<const StudyGroup*> getPendingInvites(int userId);
};
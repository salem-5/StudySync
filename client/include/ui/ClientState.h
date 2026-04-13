#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "DataStructures.h"
#include "ServerAPI.h"

class ClientState {
public:
    static std::unique_ptr<User> currentUser;
    static std::vector<StudyGroup> studyGroups;
    static std::vector<Task> tasks;
    static std::string sessionToken;
    static std::unordered_map<int, std::string> usernameCache;
    static std::vector<StudyGroup> pendingInvites;

    static std::shared_ptr<ServerAPI> apiInstance;
    static void setApi(std::shared_ptr<ServerAPI> newApi);
    static std::shared_ptr<ServerAPI> getApi();

    static const User* getUser();
    static const std::vector<StudyGroup>& getStudyGroups();
    static const std::vector<Task>& getTasks();
    static const StudyGroup* getGroupById(int id);
    static bool isGroupPinned(int groupId);
    static std::string getUsername(int userId);
    static std::vector<const StudyGroup*> getPendingInvites(int userId);

    static void loadFromPayload(const LoginPayload& payload);
    static void clear();
    static void initDummyData();

    static void togglePinGroup(int groupId);
    static void toggleTaskCompletion(int taskId, bool completed);
    static void editTask(int taskId, const std::string& title, const std::string& tag, int assigneeId);
    static void removeMemberFromGroup(int groupId, int userId);
    static void deleteGroup(int groupId);
    static void deleteTask(int taskId);
    static void cancelInvite(int groupId, int userId);
    static void acceptInvite(int groupId, int userId);
    static void denyInvite(int groupId, int userId);

    static void createGroup(const std::string& groupName);
    static void createGroup(int groupId, const std::string& groupName);

    static void createTask(int groupId, const std::string& title, const std::string& category, int assigneeId);
    static void createTask(int taskId, int groupId, const std::string& title, const std::string& category, int assigneeId);

    static void sendMessage(int groupId, const std::string& text);
    static void sendMessage(int groupId, int userId, const std::string& text);

    static void inviteMemberToGroup(int groupId, const std::string& username);
    static void inviteMemberToGroup(int groupId, int userId);

    static void createUser(const std::string& username, const std::string& email, const std::string& password);
    static void deleteUser(int userId);
    static void requestUsername(int userId, const std::string& username);
    static int getUserIdByUsername(const std::string& username);
    static void addMemberToGroup(int groupId, int userId);
};
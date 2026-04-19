#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <mutex>
#include <stdexcept>
#include "DataStructures.h"

class Database {
public:
    static Database& getInstance(); // single instance of th

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    int getSyncCounter();

    int getUserIdByUsername(const std::string& username);
    std::string getUsernameById(int userId);

    int createGroup(const std::string& groupName, int creatorId);
    void deleteGroup(int groupId);
    bool isGroupPinned(int userId, int groupId);
    void togglePinGroup(int userId, int groupId);

    void addMemberToGroup(int groupId, int userId);
    void removeMemberFromGroup(int groupId, int userId);
    void inviteMemberToGroup(int groupId, int userId);
    void cancelInvite(int groupId, int userId);
    void acceptInvite(int groupId, int userId);
    void denyInvite(int groupId, int userId);

    int createTask(int groupId, const std::string& title, const std::string& tag, int ownerId, int assigneeId);
    void toggleTaskCompletion(int taskId, bool completed);
    void editTask(int taskId, const std::string& title, const std::string& tag, int assigneeId);
    void deleteTask(int taskId);

    void addMessage(int groupId, int userId, const std::string& text);
    int createUser(const std::string& username, const std::string& email, const std::string& password);
    void deleteUser(int userId);

    bool validateLogin(const std::string& username, const std::string& password, int& outUserId);

    std::vector<int> getUsersInGroup(int groupId);

    void createTemplateForUser(int userId);
    LoginPayload getFullUserData(int userId, const std::string& sessionToken);

    void decrementAiCredits(int userId);
    int getAiCredits(int userId);
    void addAiMessage(int userId, const std::string& role, const std::string& text, const std::vector<int>& attachments);
    std::vector<AiMessage> getAiMessages(int userId);
    void clearAiMessages(int userId);
    void replaceLastAiMessage(int userId, const std::string& newText);
    void setAiCancellationFlag(int userId, bool flag);
private:
    Database();
    ~Database();

    sqlite3* db;
    std::mutex dbMutex;

    void initTables();
    void initTriggers();
    void executeQuery(const std::string& query);
};
#pragma once
#include "ClientNetworkManager.h"
#include <memory>
#include <string>
#include <functional>
#include <boost/json.hpp>

class ServerAPI {
public:
    ServerAPI(std::shared_ptr<ClientNetworkManager> networkManager);

    void login(const std::string& username, const std::string& password, std::function<void(bool, const std::string&)> callback);
    void createUser(const std::string& username, const std::string& email, const std::string& password, std::function<void(bool)> callback);
    void deleteUser(int userId, std::function<void(bool)> callback);
    void requestUsername(int userId, std::function<void(bool)> callback);

    void createGroup(const std::string& groupName, std::function<void(bool, int)> callback);
    void deleteGroup(int groupId, std::function<void(bool)> callback);
    void togglePinGroup(int groupId, std::function<void(bool)> callback);

    void addMemberToGroup(int groupId, const std::string& username, std::function<void(bool)> callback);
    void removeMemberFromGroup(int groupId, int userId, std::function<void(bool)> callback);
    void inviteMemberToGroup(int groupId, const std::string& username, std::function<void(bool)> callback);
    void cancelInvite(int groupId, int userId, std::function<void(bool)> callback);
    void acceptInvite(int groupId, int userId, std::function<void(bool)> callback);
    void denyInvite(int groupId, int userId, std::function<void(bool)> callback);

    void createTask(int groupId, const std::string& title, const std::string& category, int assigneeId, std::function<void(bool, int)> callback);
    void editTask(int taskId, const std::string& title, const std::string& tag, int assigneeId, std::function<void(bool)> callback);
    void deleteTask(int taskId, std::function<void(bool)> callback);
    void toggleTaskCompletion(int taskId, bool completed, std::function<void(bool)> callback);

    void sendMessage(int groupId, const std::string& text, std::function<void(bool)> callback);

private:
    std::shared_ptr<ClientNetworkManager> network;
};
#include "ServerAPI.h"
#include "ui/ClientState.h"
#include "DataStructures.h"

ServerAPI::ServerAPI(std::shared_ptr<ClientNetworkManager> networkManager)
    : network(std::move(networkManager)) {}

void ServerAPI::login(const std::string& username, const std::string& password, std::function<void(bool, const std::string&)> callback) {
    boost::json::object payload = { {"username", username}, {"password", password} };
    network->sendRequest("login", payload, [callback](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        std::string msg = response.contains("message") ? response.at("message").as_string().c_str() : "";
        if (success && response.contains("payload")) {
            try {
                LoginPayload loginData = LoginPayload::fromJson(response.at("payload").as_object());
                ClientState::loadFromPayload(loginData);
            } catch (const std::exception& e) {
                success = false;
                msg = "Data parsing error: " + std::string(e.what());
            }
        }
        callback(success, msg);
    });
}

void ServerAPI::createUser(const std::string& username, const std::string& email, const std::string& password, std::function<void(bool)> callback) {
    boost::json::object payload = { {"username", username}, {"email", email}, {"password", password} };
    network->sendRequest("createUser", payload, [callback](const boost::json::object& response) {
        callback(response.contains("status") && response.at("status").as_string() == "success");
    });
}

void ServerAPI::deleteUser(int userId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"userId", userId} };
    network->sendRequest("deleteUser", payload, [callback, userId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::deleteUser(userId);
        if (callback) callback(success);
    });
}

void ServerAPI::togglePinGroup(int groupId, std::function<void(bool)> callback) {
    if (!ClientState::getUser()) return;
    boost::json::object payload = { {"userId", ClientState::getUser()->getId()}, {"groupId", groupId} };
    network->sendRequest("togglePinGroup", payload, [callback, groupId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::togglePinGroup(groupId);
        if (callback) callback(success);
    });
}

void ServerAPI::createGroup(const std::string& groupName, std::function<void(bool, int)> callback) {
    if (!ClientState::getUser()) return;
    boost::json::object payload = { {"groupName", groupName}, {"creatorId", ClientState::getUser()->getId()} };
    network->sendRequest("createGroup", payload, [callback, groupName](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        int groupId = success ? response.at("groupId").as_int64() : -1;
        if (success) ClientState::createGroup(groupId, groupName);
        if (callback) callback(success, groupId);
    });
}

void ServerAPI::deleteGroup(int groupId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId} };
    network->sendRequest("deleteGroup", payload, [callback, groupId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::deleteGroup(groupId);
        if (callback) callback(success);
    });
}

void ServerAPI::createTask(int groupId, const std::string& title, const std::string& category, int assigneeId, std::function<void(bool, int)> callback) {
    if (!ClientState::getUser()) return;
    boost::json::object payload = {
        {"groupId", groupId}, {"title", title}, {"tag", category},
        {"ownerId", ClientState::getUser()->getId()}, {"assigneeId", assigneeId}
    };
    network->sendRequest("createTask", payload, [callback, groupId, title, category, assigneeId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        int taskId = success ? response.at("taskId").as_int64() : -1;
        if (success) ClientState::createTask(taskId, groupId, title, category, assigneeId);
        if (callback) callback(success, taskId);
    });
}

void ServerAPI::toggleTaskCompletion(int taskId, bool completed, std::function<void(bool)> callback) {
    boost::json::object payload = { {"taskId", taskId}, {"completed", completed} };
    network->sendRequest("toggleTaskCompletion", payload, [callback, taskId, completed](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::toggleTaskCompletion(taskId, completed);
        if (callback) callback(success);
    });
}

void ServerAPI::editTask(int taskId, const std::string& title, const std::string& tag, int assigneeId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"taskId", taskId}, {"title", title}, {"tag", tag}, {"assigneeId", assigneeId} };
    network->sendRequest("editTask", payload, [callback, taskId, title, tag, assigneeId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::editTask(taskId, title, tag, assigneeId);
        if (callback) callback(success);
    });
}

void ServerAPI::deleteTask(int taskId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"taskId", taskId} };
    network->sendRequest("deleteTask", payload, [callback, taskId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::deleteTask(taskId);
        if (callback) callback(success);
    });
}

void ServerAPI::sendMessage(int groupId, const std::string& text, std::function<void(bool)> callback) {
    if (!ClientState::getUser()) return;
    int userId = ClientState::getUser()->getId();
    boost::json::object payload = { {"groupId", groupId}, {"userId", userId}, {"text", text} };
    network->sendRequest("sendMessage", payload, [callback, groupId, userId, text](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::sendMessage(groupId, userId, text);
        if (callback) callback(success);
    });
}

void ServerAPI::requestUsername(int userId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"userId", userId} };
    network->sendRequest("getUsername", payload, [callback, userId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success && response.contains("username")) {
            ClientState::requestUsername(userId, response.at("username").as_string().c_str());
        }
        if (callback) callback(success);
    });
}

void ServerAPI::addMemberToGroup(int groupId, const std::string& username, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId}, {"username", username} };
    network->sendRequest("addMemberToGroup", payload, [callback, groupId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success && response.contains("userId")) {
            ClientState::addMemberToGroup(groupId, response.at("userId").as_int64());
        }
        if (callback) callback(success);
    });
}

void ServerAPI::removeMemberFromGroup(int groupId, int userId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId}, {"userId", userId} };
    network->sendRequest("removeMemberFromGroup", payload, [callback, groupId, userId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::removeMemberFromGroup(groupId, userId);
        if (callback) callback(success);
    });
}

void ServerAPI::inviteMemberToGroup(int groupId, const std::string& username, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId}, {"username", username} };
    network->sendRequest("inviteMemberToGroup", payload, [callback, groupId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success && response.contains("userId")) {
            ClientState::inviteMemberToGroup(groupId, response.at("userId").as_int64());
        }
        if (callback) callback(success);
    });
}

void ServerAPI::cancelInvite(int groupId, int userId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId}, {"userId", userId} };
    network->sendRequest("cancelInvite", payload, [callback, groupId, userId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::cancelInvite(groupId, userId);
        if (callback) callback(success);
    });
}

void ServerAPI::acceptInvite(int groupId, int userId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId}, {"userId", userId} };
    network->sendRequest("acceptInvite", payload, [callback, groupId, userId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::acceptInvite(groupId, userId);
        if (callback) callback(success);
    });
}

void ServerAPI::denyInvite(int groupId, int userId, std::function<void(bool)> callback) {
    boost::json::object payload = { {"groupId", groupId}, {"userId", userId} };
    network->sendRequest("denyInvite", payload, [callback, groupId, userId](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        if (success) ClientState::denyInvite(groupId, userId);
        if (callback) callback(success);
    });
}

void ServerAPI::disconnect() { network->disconnect(); }
void ServerAPI::setServerAddress(const std::string& host, const std::string& port) {
    network->setServerAddress(host, port);
}
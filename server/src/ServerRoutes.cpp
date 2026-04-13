#include "MessageRouter.h"
#include "Database.h"
#include <ctime>
#include <string>

void registerServerRoutes() {
    auto& router = MessageRouter::getInstance();

    router.registerRoute("ping", [](const boost::json::object& req, boost::json::object& res) {
        res["status"] = "success";
    });

    router.registerRoute("time", [](const boost::json::object& req, boost::json::object& res) {
        std::time_t now = std::time(0);
        std::string timeStr = std::ctime(&now);
        if (!timeStr.empty() && timeStr.back() == '\n') timeStr.pop_back();

        res["data"] = timeStr;
        res["status"] = "success";
    });

    router.registerRoute("createUser", [](const boost::json::object& req, boost::json::object& res) {
        std::string username = req.at("username").as_string().c_str();
        std::string email = req.at("email").as_string().c_str();
        std::string password = req.at("password").as_string().c_str();

        int newId = Database::getInstance().createUser(username, email, password);
        if (newId != -1) {
            Database::getInstance().createTemplateForUser(newId);
            res["status"] = "success";
            res["message"] = "User created successfully";
            res["sync_counter"] = Database::getInstance().getSyncCounter();
        } else {
            res["status"] = "error";
            res["message"] = "Username already exists.";
        }
    });

    router.registerRoute("login", [](const boost::json::object& req, boost::json::object& res) {
        std::string username = req.at("username").as_string().c_str();
        std::string password = req.at("password").as_string().c_str();
        int userId = -1;

        if (Database::getInstance().validateLogin(username, password, userId)) {
            res["status"] = "success";
            std::string token = "session_" + std::to_string(userId) + "_" + std::to_string(time(0));
            LoginPayload payload = Database::getInstance().getFullUserData(userId, token);
            res["payload"] = payload.toJson();
            res["sync_counter"] = Database::getInstance().getSyncCounter();
        } else {
            res["status"] = "error";
            res["message"] = "Invalid username or password.";
        }
    });

    router.registerRoute("deleteUser", [](const boost::json::object& req, boost::json::object& res) {
        int userId = req.at("userId").as_int64();
        Database::getInstance().deleteUser(userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("togglePinGroup", [](const boost::json::object& req, boost::json::object& res) {
        int userId = req.at("userId").as_int64();
        int groupId = req.at("groupId").as_int64();
        Database::getInstance().togglePinGroup(userId, groupId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("createGroup", [](const boost::json::object& req, boost::json::object& res) {
        std::string name = req.at("groupName").as_string().c_str();
        int creatorId = req.at("creatorId").as_int64();
        int newId = Database::getInstance().createGroup(name, creatorId);

        if (newId != -1) {
            res["status"] = "success";
            res["groupId"] = newId;
            res["sync_counter"] = Database::getInstance().getSyncCounter();
        } else {
            res["status"] = "error";
            res["message"] = "Failed to create group.";
        }
    });

    router.registerRoute("deleteGroup", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        Database::getInstance().deleteGroup(groupId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("createTask", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        std::string title = req.at("title").as_string().c_str();
        std::string tag = req.at("tag").as_string().c_str();
        int ownerId = req.at("ownerId").as_int64();
        int assigneeId = req.at("assigneeId").as_int64();

        int newId = Database::getInstance().createTask(groupId, title, tag, ownerId, assigneeId);
        if (newId != -1) {
            res["status"] = "success";
            res["taskId"] = newId;
            res["sync_counter"] = Database::getInstance().getSyncCounter();
        } else {
            res["status"] = "error";
        }
    });

    router.registerRoute("toggleTaskCompletion", [](const boost::json::object& req, boost::json::object& res) {
        int taskId = req.at("taskId").as_int64();
        bool completed = req.at("completed").as_bool();
        Database::getInstance().toggleTaskCompletion(taskId, completed);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("editTask", [](const boost::json::object& req, boost::json::object& res) {
        int taskId = req.at("taskId").as_int64();
        std::string title = req.at("title").as_string().c_str();
        std::string tag = req.at("tag").as_string().c_str();
        int assigneeId = req.at("assigneeId").as_int64();
        Database::getInstance().editTask(taskId, title, tag, assigneeId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("deleteTask", [](const boost::json::object& req, boost::json::object& res) {
        int taskId = req.at("taskId").as_int64();
        Database::getInstance().deleteTask(taskId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("sendMessage", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        std::string text = req.at("text").as_string().c_str();
        Database::getInstance().addMessage(groupId, userId, text);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("getUsername", [](const boost::json::object& req, boost::json::object& res) {
        int userId = req.at("userId").as_int64();
        std::string username = Database::getInstance().getUsernameById(userId);
        if (!username.empty()) {
            res["status"] = "success";
            res["username"] = username;
        } else {
            res["status"] = "error";
        }
    });

    router.registerRoute("addMemberToGroup", [](const boost::json::object& req, boost::json::object& res) {
        try {
            if (!req.contains("groupId") || !req.contains("username")) {
                res["status"] = "error";
                res["message"] = "Missing parameters";
                return;
            }

            int groupId = req.at("groupId").as_int64();
            std::string username = req.at("username").as_string().c_str();
            int userId = Database::getInstance().getUserIdByUsername(username);

            if (userId != -1) {
                Database::getInstance().addMemberToGroup(groupId, userId);
                res["status"] = "success";
                res["userId"] = userId;
                res["sync_counter"] = Database::getInstance().getSyncCounter();
            } else {
                res["status"] = "error";
                res["message"] = "User not found";
            }
        } catch (const std::exception& e) {
            res["status"] = "error";
            res["message"] = e.what();
        }
    });

    router.registerRoute("removeMemberFromGroup", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().removeMemberFromGroup(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("inviteMemberToGroup", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        std::string username = req.at("username").as_string().c_str();
        int userId = Database::getInstance().getUserIdByUsername(username);

        if (userId != -1) {
            Database::getInstance().inviteMemberToGroup(groupId, userId);
            res["status"] = "success";
            res["userId"] = userId;
            res["sync_counter"] = Database::getInstance().getSyncCounter();
        } else {
            res["status"] = "error";
            res["message"] = "User not found.";
        }
    });

    router.registerRoute("cancelInvite", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().cancelInvite(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("acceptInvite", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().acceptInvite(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });

    router.registerRoute("denyInvite", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().denyInvite(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();
    });
}
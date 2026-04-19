#include "MessageRouter.h"
#include "Database.h"
#include <string>
#include <iostream>
#include "ConnectionManager.h"
#include <boost/asio/steady_timer.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <memory>
#include <regex>
#include "TcpClient.h"

static std::unique_ptr<TcpClient> aiPythonClient = nullptr;

void pushUpdateToUser(int userId) {
    auto conn = ConnectionManager::getInstance().getConnection(userId);
    if (conn) {
        std::string token = "session_" + std::to_string(userId) + "_" + std::to_string(time(0));
        LoginPayload payload = Database::getInstance().getFullUserData(userId, token);

        boost::json::object msg;
        msg["cmd"] = "server_push_update";
        msg["payload"] = payload.toJson();

        conn->send(boost::json::serialize(msg));
    }
}

void pushUpdateToGroup(int groupId, int excludeUserId = -1) {
    std::vector<int> users = Database::getInstance().getUsersInGroup(groupId);
    for (int uid : users) {
        if (uid != excludeUserId) {
            pushUpdateToUser(uid);
        }
    }
}

void pushUpdateToAll() {
    std::vector<int> activeUsers = ConnectionManager::getInstance().getActiveUserIds();
    for (int uid : activeUsers) {
        pushUpdateToUser(uid);
    }
}

void startAutoRefreshTimer(boost::asio::io_context& io_context) {
    static std::shared_ptr<boost::asio::steady_timer> timer =
        std::make_shared<boost::asio::steady_timer>(io_context, std::chrono::seconds(5));

    timer->expires_after(std::chrono::seconds(5));
    timer->async_wait([&io_context](const boost::system::error_code& ec) {
        if (!ec) {
            pushUpdateToAll();
            startAutoRefreshTimer(io_context);
        }
    });
}

void registerServerRoutes(int port) {
    auto& router = MessageRouter::getInstance();

    aiPythonClient = std::make_unique<TcpClient>("127.0.0.1", std::to_string(port),
        [](const std::string& msg) {
            try {
                auto parsed = boost::json::parse(msg).as_object();
                int userId = parsed.at("user_id").as_int64();
                std::string status = parsed.at("status").as_string().c_str();
                std::string aiMessage = parsed.at("message").as_string().c_str();

                if (status == "success") {
                    Database::getInstance().addAiMessage(userId, "ai", aiMessage, {});
                }
                auto conn = ConnectionManager::getInstance().getConnection(userId);
                if (conn) {
                    boost::json::object response;
                    response["cmd"] = "ai_response";
                    response["success"] = (status == "success");
                    response["message"] = aiMessage;
                    conn->send(boost::json::serialize(response));
                }
                pushUpdateToUser(userId);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing AI response: " << e.what() << "\n";
            }
        },
        [port]() { std::cout << "Connected to AI Python Backend on " << port << "\n"; }
    );

    router.registerRoute("askAi", [](const boost::json::object& req, boost::json::object& res) {
        int userId = req.at("userId").as_int64();
        std::string text = req.at("text").as_string().c_str();

        std::vector<int> taskIds;
        if (req.contains("taskIds")) {
            for (const auto& item : req.at("taskIds").as_array()) {
                taskIds.push_back(item.as_int64());
            }
        }

        if (Database::getInstance().getAiCredits(userId) <= 0) {
            res["status"] = "error";
            res["message"] = "No AI credits remaining.";
            return;
        }

        Database::getInstance().decrementAiCredits(userId);
        Database::getInstance().addAiMessage(userId, "user", text, taskIds);
        pushUpdateToUser(userId);

        if (aiPythonClient && aiPythonClient->isConnected()) {
            boost::json::object pyReq;
            pyReq["user_id"] = userId;
            pyReq["req_id"] = req.contains("req_id") ? req.at("req_id").as_int64() : 0;
            pyReq["current_message"] = text;

            std::string dummyToken = "";
            LoginPayload payload = Database::getInstance().getFullUserData(userId, dummyToken);

            boost::json::array historyArr;
            const auto& allMessages = payload.getAiMessages();
            for (size_t i = 0; i < allMessages.size(); ++i) {
                if (allMessages[i].getRole() == "user") {
                    if (i + 1 < allMessages.size() && allMessages[i+1].getText() == "Response cancelled by user") {
                        continue;
                    }
                    boost::json::object mObj;
                    mObj["role"] = allMessages[i].getRole();
                    mObj["content"] = allMessages[i].getText();
                    historyArr.push_back(mObj);
                }
                else if (allMessages[i].getRole() == "ai" || allMessages[i].getRole() == "assistant") {
                    if (allMessages[i].getText() == "Response cancelled by user") {
                        continue;
                    }
                    boost::json::object mObj;
                    mObj["role"] = allMessages[i].getRole();
                    mObj["content"] = allMessages[i].getText();
                    historyArr.push_back(mObj);
                }
            }
            pyReq["message_history"] = historyArr;

            boost::json::array tasksArr;
            for (int id : taskIds) {
                for (const auto& t : payload.getTasks()) {
                    if (t.getId() == id) {
                        boost::json::object taskObj;
                        taskObj["id"] = t.getId();
                        taskObj["title"] = t.getTitle();
                        taskObj["tag"] = t.getTag();
                        taskObj["groupId"] = t.getGroupId();
                        tasksArr.push_back(taskObj);
                        break;
                    }
                }
            }
            pyReq["attached_tasks"] = tasksArr;

            aiPythonClient->send(boost::json::serialize(pyReq));

            res["status"] = "success";
        } else {
            res["status"] = "error";
            res["message"] = "AI Backend is offline.";
        }
    });

    router.registerRoute("clearAiHistory", [](const boost::json::object& req, boost::json::object& res) {
            int userId = req.at("userId").as_int64();
            Database::getInstance().clearAiMessages(userId);
            res["status"] = "success";
            pushUpdateToUser(userId);
        });

    router.registerRoute("cancelAi", [](const boost::json::object& req, boost::json::object& res) {
        int userId = req.at("userId").as_int64();
        auto msgs = Database::getInstance().getAiMessages(userId);

        if (!msgs.empty()) {
            if (msgs.back().getRole() == "ai" && msgs.back().getText() != "Response cancelled by user") {
                Database::getInstance().replaceLastAiMessage(userId, "Response cancelled by user");
            } else if (msgs.back().getRole() == "user") {
                Database::getInstance().addAiMessage(userId, "ai", "Response cancelled by user", {});
            }
        }
        Database::getInstance().setAiCancellationFlag(userId, true);

        res["status"] = "success";
        pushUpdateToUser(userId);
    });

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
        std::regex emailRegex(R"(.+@.+\..+)");
        if (!std::regex_match(email, emailRegex)) {
            res["status"] = "error";
            res["message"] = "Invalid email format.";
            return;
        }
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
            res["userId"] = userId;
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

        pushUpdateToUser(userId);
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
        std::vector<int> usersToNotify = Database::getInstance().getUsersInGroup(groupId);
        Database::getInstance().deleteGroup(groupId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();

        for (int uid : usersToNotify) {
            pushUpdateToUser(uid);
        }
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

        pushUpdateToGroup(groupId, req.at("ownerId").as_int64());
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

        pushUpdateToGroup(groupId, userId);
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

                pushUpdateToGroup(groupId);
                pushUpdateToUser(userId);
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

        pushUpdateToGroup(groupId);
        pushUpdateToUser(userId);
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

        pushUpdateToUser(userId);
    });

    router.registerRoute("cancelInvite", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().cancelInvite(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();

        pushUpdateToUser(userId);
    });

    router.registerRoute("acceptInvite", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().acceptInvite(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();

        pushUpdateToGroup(groupId);
    });

    router.registerRoute("denyInvite", [](const boost::json::object& req, boost::json::object& res) {
        int groupId = req.at("groupId").as_int64();
        int userId = req.at("userId").as_int64();
        Database::getInstance().denyInvite(groupId, userId);
        res["status"] = "success";
        res["sync_counter"] = Database::getInstance().getSyncCounter();

        pushUpdateToUser(userId);
    });
}
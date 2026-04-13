#include "ClientNetworkManager.h"
#include <iostream>
#include "ui/MainWindow.h"
ClientNetworkManager::ClientNetworkManager(const std::string& host, const std::string& port)
    : tcpClient(host, port,
        [this](const std::string& msg) { onMessageReceived(msg); },
        [this]() { if (on_connect_callback_) on_connect_callback_(); }
      ) {}

void ClientNetworkManager::setOnConnectCallback(std::function<void()> callback) {
    on_connect_callback_ = std::move(callback);
}

void ClientNetworkManager::onMessageReceived(const std::string& message) {
    try {
        boost::json::value parsed = boost::json::parse(message);
        boost::json::object obj = parsed.as_object();
        if (obj.contains("req_id")) {
            int req_id = obj.at("req_id").as_int64();
            std::function<void(const boost::json::object&)> callback;
            {
                std::lock_guard<std::mutex> lock(callbacksMutex);
                auto it = pending_requests_.find(req_id);
                if (it != pending_requests_.end()) {
                    callback = it->second;
                    pending_requests_.erase(it); // Remove it once handled
                }
            }

            if (callback) {
                callback(obj);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "failed to parse json response: " << e.what() << std::endl;
    }
}

void ClientNetworkManager::sendRequest(const std::string& command, boost::json::object payload, std::function<void(const boost::json::object&)> callback) {
    int reqId = nextRequestId++;
    payload["req_id"] = reqId;
    payload["cmd"] = command;

    // store the callback so we trigger it when server replies with reqId
    {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        pending_requests_[reqId] = callback;
    }

    tcpClient.send(boost::json::serialize(payload));
}

void ClientNetworkManager::ping(std::function<void(bool)> callback) {
    boost::json::object payload;

    sendRequest("ping", payload, [callback](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        callback(success);
    });
}
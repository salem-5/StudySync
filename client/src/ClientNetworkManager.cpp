#include "ClientNetworkManager.h"
#include <iostream>
#include <thread> // Added for timeout handling
#include <chrono> // Added for timeout handling

#include "ui/ClientState.h"
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

        if (obj.contains("cmd") && obj.at("cmd").as_string() == "server_push_update") {
            if (obj.contains("payload")) {
                try {
                    LoginPayload payload = LoginPayload::fromJson(obj.at("payload").as_object());
                    ClientState::loadFromPayload(payload);

                    if (ClientNotifier::instance()) {
                        emit ClientNotifier::instance()->groupsChanged();
                        emit ClientNotifier::instance()->tasksChanged();
                        emit ClientNotifier::instance()->invitesChanged();
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load push update payload: " << e.what() << std::endl;
                }
            }
            return;
        }

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

    std::weak_ptr<ClientNetworkManager> weakSelf = weak_from_this();
    std::thread([weakSelf, reqId]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (auto self = weakSelf.lock()) {
            std::function<void(const boost::json::object&)> timeoutCb;
            {
                std::lock_guard<std::mutex> lock(self->callbacksMutex);
                auto it = self->pending_requests_.find(reqId);
                if (it != self->pending_requests_.end()) {
                    timeoutCb = it->second;
                    self->pending_requests_.erase(it);
                }
            }
            if (timeoutCb) {
                boost::json::object errorPayload;
                errorPayload["req_id"] = reqId;
                errorPayload["status"] = "error";
                errorPayload["message"] = "Connection timed out after 5 seconds.";
                timeoutCb(errorPayload);
            }
        }
    }).detach();
}
void ClientNetworkManager::ping(std::function<void(bool)> callback) {
    boost::json::object payload;

    sendRequest("ping", payload, [callback](const boost::json::object& response) {
        bool success = response.contains("status") && response.at("status").as_string() == "success";
        callback(success);
    });
}

void ClientNetworkManager::disconnect() { tcpClient.disconnect(); }
void ClientNetworkManager::setServerAddress(const std::string& host, const std::string& port) {
    tcpClient.setServerAddress(host, port);
}
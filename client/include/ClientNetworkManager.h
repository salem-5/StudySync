#pragma once
#pragma once
#include "TcpClient.h"
#include <boost/json.hpp>
#include <unordered_map>
#include <atomic>

class ClientNetworkManager : public std::enable_shared_from_this<ClientNetworkManager> {
public:
    ClientNetworkManager(const std::string& host, const std::string& port);

    void setOnConnectCallback(std::function<void()> callback);
    // endpoints like a rest api
    void ping(std::function<void(bool success)> callback);

    void disconnect();

    void setServerAddress(const std::string &host, const std::string &port);

    void onMessageReceived(const std::string& message);
    void sendRequest(const std::string& command, boost::json::object payload, std::function<void(const boost::json::object&)> callback);

private:
    TcpClient tcpClient;
    std::atomic<int> nextRequestId{1};

    // taken from youtube video, maps id to the callback to be run
    std::mutex callbacksMutex;
    std::unordered_map<int, std::function<void(const boost::json::object&)>> pending_requests_;
    std::function<void()> on_connect_callback_;
};
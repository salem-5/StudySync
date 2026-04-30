#pragma once

#include <boost/json.hpp>
#include <functional>
#include <unordered_map>
#include <string>

using CommandHandler = std::function<void(const boost::json::object& request, boost::json::object& response)>; // for lambda callbacks made by chatgpt

class MessageRouter {
public:
    static MessageRouter& getInstance();

    void registerRoute(const std::string& cmd, CommandHandler handler);

    bool handle(const std::string& cmd,
                const boost::json::object& request,
                boost::json::object& response);
    void clearRoutes();
private:
    MessageRouter() = default;

    std::unordered_map<std::string, CommandHandler> routes_;
};
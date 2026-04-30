#include "MessageRouter.h"

MessageRouter& MessageRouter::getInstance() {
    static MessageRouter instance;
    return instance;
}

void MessageRouter::registerRoute(const std::string& cmd, CommandHandler handler) {
    routes_[cmd] = std::move(handler);
}

bool MessageRouter::handle(const std::string& cmd, const boost::json::object& request, boost::json::object& response) {
    auto it = routes_.find(cmd);
    if (it != routes_.end()) {
        it->second(request, response);
        return true;
    }
    return false;
}

void MessageRouter::clearRoutes() {
    routes_.clear();
}
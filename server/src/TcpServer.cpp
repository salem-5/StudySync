#include "TcpServer.h"
#include "MessageRouter.h"
#include <iostream>
#include <istream>
#include <boost/json.hpp>

#include "ConnectionManager.h"

TcpConnection::pointer TcpConnection::create(boost::asio::io_context& ioContext) {
    return pointer(new TcpConnection(ioContext));
}

TcpConnection::TcpConnection(boost::asio::io_context& io_context) : socketObject(io_context) {}

tcp::socket& TcpConnection::socket() {
    return socketObject;
}

void TcpConnection::start() {
    doRead();
}

void TcpConnection::doRead() {
    auto self(shared_from_this());
    boost::asio::async_read_until(socketObject, readBuffer, '\n',
        [this, self](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                std::istream is(&readBuffer);
                std::string message;
                std::getline(is, message);
                handleMessage(message);
                doRead();
            } else {
                std::cout << "client disconnected: " << ec.message() << std::endl;
                if (this->userId != -1) {
                    ConnectionManager::getInstance().removeConnection(this->userId);
                }
            }
        });
}

void TcpConnection::handleMessage(const std::string& msg) {
    try {
        boost::json::value parsed = boost::json::parse(msg);
        boost::json::object obj = parsed.as_object();

        if (!obj.contains("req_id") || !obj.contains("cmd")) return;

        int req_id = obj.at("req_id").as_int64();
        std::string cmd = obj.at("cmd").as_string().c_str();

        boost::json::object response;
        response["req_id"] = req_id;

        if (!MessageRouter::getInstance().handle(cmd, obj, response)) {
            response["status"] = "error";
            response["message"] = "Unknown command";
        }

        if (cmd == "login" && response.contains("status") && response.at("status").as_string() == "success") {
            if (response.contains("userId")) { // We'll add this to the login route next
                this->userId = response.at("userId").as_int64();
                ConnectionManager::getInstance().addConnection(this->userId, shared_from_this());
            }
        }

        send(boost::json::serialize(response));

    } catch (const std::exception& e) {
        std::cerr << "error parsing the json " << e.what() << std::endl;
    }
}
void TcpConnection::send(const std::string& message) {
    std::string formatted_msg = message;
    if (formatted_msg.back() != '\n') formatted_msg += '\n';

    auto self(shared_from_this());
    boost::asio::post(socketObject.get_executor(), [this, self, formatted_msg]() {
        bool write_in_progress = !writeQueue.empty();
        writeQueue.push(formatted_msg);
        if (!write_in_progress) {
            doWrite();
        }
    });
}

void TcpConnection::doWrite() {
    auto self(shared_from_this());
    boost::asio::async_write(socketObject,
        boost::asio::buffer(writeQueue.front()),
        [this, self](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                writeQueue.pop();
                if (!writeQueue.empty()) doWrite();
            } else {
                std::cerr << "write error: " << ec.message() << std::endl;
            }
        });
}

TcpServer::TcpServer(boost::asio::io_context& ioContext, int port)
    : io_context_(ioContext), acceptor(ioContext, tcp::endpoint(tcp::v4(), port)) {
    startAccept();
}

void TcpServer::startAccept() {
    TcpConnection::pointer new_connection = TcpConnection::create(io_context_);
    acceptor.async_accept(new_connection->socket(),
        [this, new_connection](const boost::system::error_code& error) {
            this->handleAccept(new_connection, error);
        });
}

void TcpServer::handleAccept(TcpConnection::pointer new_connection, const boost::system::error_code& error) {
    if (!error) {
        new_connection->start();
    } else {
        std::cerr << "Accept error: " << error.message() << std::endl;
    }
    startAccept();
}
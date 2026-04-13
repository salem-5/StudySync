#include "TcpClient.h"
#include <iostream>
#include <istream>
#include <boost/json/src.hpp>


TcpClient::TcpClient(const std::string& host, const std::string& port, MessageHandler handler, ConnectHandler on_connect)
    : socket(ioContext), resolver(ioContext), reconnectTimer(ioContext),
      host(host), port(port), onMessage(std::move(handler)), onConnect(std::move(on_connect)) {
    connect();
    // based of chatgpt, run the io context on a seperate thread to not block the qt thread
    ioThread = std::thread([this]() { ioContext.run(); });
}

TcpClient::~TcpClient() {
    ioContext.stop();
    if (ioThread.joinable()) ioThread.join();
}

bool TcpClient::isConnected() const { return connected; }

void TcpClient::send(const std::string& msg) {
    // ensure the message ends with a newline delimiter
    std::string formatted_msg = msg;
    if (formatted_msg.back() != '\n') formatted_msg += '\n';

    // push to the queueeu
    boost::asio::post(ioContext, [this, formatted_msg]() {
        bool write_in_progress = !writeQueue.empty();
        writeQueue.push(formatted_msg);
        if (!write_in_progress && connected) {
            doWrite();
        }
    });
}

void TcpClient::disconnect() {
    boost::asio::post(ioContext, [this]() {
        connected = false;
        boost::system::error_code ec;
        socket.close(ec);
        reconnectTimer.cancel();

        std::queue<std::string> emptyQueue;
        std::swap(writeQueue, emptyQueue);
    });
}

void TcpClient::setServerAddress(const std::string& new_host, const std::string& new_port) {
    boost::asio::post(ioContext, [this, new_host, new_port]() {
        host = new_host;
        port = new_port;
        connected = false;
        boost::system::error_code ec;
        socket.close(ec);
        reconnectTimer.cancel();

        std::queue<std::string> emptyQueue;
        std::swap(writeQueue, emptyQueue);

        connect();
    });
}

void TcpClient::connect() {
    std::cout << "connecting to " << host << ":" << port << std::endl;
    resolver.async_resolve(host, port,
        [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results) {
            if (!ec) {
                boost::asio::async_connect(socket, results,
                    [this](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint) {
                        if (!ec) {
                            std::cout << "connected successfully." << std::endl;
                            connected = true;
                            if (onConnect) onConnect();
                            doRead();
                            if (!writeQueue.empty()) doWrite();
                        } else {
                            checkReconnect();
                        }
                    });
            } else {
                checkReconnect();
            }
        });
}

void TcpClient::checkReconnect() {
    connected = false;
    socket.close();
    // reconnect every 3 seconds
    reconnectTimer.expires_after(std::chrono::seconds(3));
    reconnectTimer.async_wait([this](const boost::system::error_code& ec) {
        if (!ec) connect();
    });
}

void TcpClient::doRead() {
    // Read until a newline is found
    boost::asio::async_read_until(socket, readBuffer, '\n',
        [this](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                std::istream is(&readBuffer);
                std::string message;
                std::getline(is, message); // extracts up to '\n'
                
                if (onMessage) onMessage(message);
                
                doRead(); // Queue the next read
            } else {
                std::cerr << "client disconnected: " << ec.message() << std::endl;
                checkReconnect();
            }
        });
}

void TcpClient::doWrite() {
    boost::asio::async_write(socket,
        boost::asio::buffer(writeQueue.front()),
        [this](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                writeQueue.pop();
                if (!writeQueue.empty()) doWrite(); // write the next one
            } else {
                checkReconnect();
            }
        });
}
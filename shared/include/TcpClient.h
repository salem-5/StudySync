#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>

// with much help from these sources
// https://stackoverflow.com/questions/77880466/boost-asio-async-client
// https://github.com/alejandrofsevilla/boost-tcp-server-client/blob/main/TcpClient.hpp
// also partly modified by chatgpt in the TcpClient constructor
class TcpClient {
public:
    // the callback is run when a string that ends in a \n is found
    using MessageHandler = std::function<void(const std::string&)>;
    using ConnectHandler = std::function<void()>;

    TcpClient(const std::string& host, const std::string& port, MessageHandler handler, ConnectHandler on_connect = nullptr);
    ~TcpClient();

    void send(const std::string& msg);
    bool isConnected() const;
    void setServerAddress(const std::string& new_host, const std::string& new_port);
    void disconnect();

private:
    void connect();
    void doRead();
    void doWrite();
    void checkReconnect();

    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::steady_timer reconnectTimer;
    std::thread ioThread;

    std::string host;
    std::string port;
    MessageHandler onMessage;
    ConnectHandler onConnect;
    bool connected = false;

    boost::asio::streambuf readBuffer;
    std::queue<std::string> writeQueue;
    std::mutex writeMutex;
};
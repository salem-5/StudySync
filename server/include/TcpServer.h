#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <boost/json.hpp>
#include <queue>
#include <string>


//Documentation Used: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime3.html
//Basic Tcp[ server as per the boost::asio documentations
//Slightly refactored to suite my style better
using boost::asio::ip::tcp;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    int userId = -1;
    using pointer = std::shared_ptr<TcpConnection>;

    static pointer create(boost::asio::io_context& ioContext);
    tcp::socket& socket();
    void start();
    virtual void send(const std::string& message);

    void handleMessage(const std::string& msg);

protected:
    TcpConnection(boost::asio::io_context& io_context);

private:
    void doRead();
    void doWrite();

    tcp::socket socketObject;
    boost::asio::streambuf readBuffer;
    std::queue<std::string> writeQueue;
};

class TcpServer {
public:
    TcpServer(boost::asio::io_context& ioContext, int port);
    void startAccept();
private:
    void handleAccept(TcpConnection::pointer new_connection, const boost::system::error_code& error);
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor;
};
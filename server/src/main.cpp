#include <iostream>
#include <TcpServer.h>
#include "Database.h"

int main(int argc, char* argv[]) {
    std::cout << "Server Started..." << std::endl;
    try {
        Database::getInstance();
        std::cout << "Database initialized" << std::endl;
        boost::asio::io_context io_context;
        TcpServer server(io_context, 8080);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
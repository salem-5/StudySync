#include <iostream>
#include <boost/asio.hpp>
#include <TcpServer.h>
#include <cstdint>
#include "Database.h"
#include "CmdParser.h"

void registerServerRoutes();
void startAutoRefreshTimer(boost::asio::io_context& io_context);

int main(int argc, char* argv[]) {
    Config config = parseCommandLine(argc, argv);
    uint16_t port = config.port;
    uint16_t aiPort = config.aiListenerPort;
    std::cout << "Server Starting on port " << port << "..." << std::endl;

    try {
        Database::getInstance();
        std::cout << "Database initialized" << std::endl;
        registerServerRoutes(aiPort);

        boost::asio::io_context io_context;
        TcpServer server(io_context, port);

        startAutoRefreshTimer(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "System Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
#include "CmdParser.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

void printHelp() {
    std::cout << "Allowed options:\n"
        << "  -h, --help              produce help message\n"
        << "  -p, --port <num>        set server port (default: 2452)\n"
        << "  -i, --aiport <num>      set AI listener port (default: 2570)\n";
}

static uint16_t parsePort(const std::string& value, const std::string& name) {
    try {
        unsigned long port = std::stoul(value);
        if (port > 65535) {
            throw std::out_of_range("Port out of range");
        }
        return static_cast<uint16_t>(port);
    } catch (...) {
        throw std::runtime_error("Invalid value for " + name + ": " + value);
    }
}

Config parseCommandLine(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "-h" || arg == "--help") {
            config.showHelp = true;
            printHelp();
            return config;
        }
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 >= args.size()) {
                throw std::runtime_error("--port requires a value");
            }
            config.port = parsePort(args[++i], "--port");
        }
        else if (arg == "-i" || arg == "--aiport") {
            if (i + 1 >= args.size()) {
                throw std::runtime_error("--aiport requires a value");
            }
            config.aiListenerPort = parsePort(args[++i], "--aiport");
        }
        else {
            throw std::runtime_error("Unknown option: " + arg);
        }
    }
    return config;
}
#include "CmdParser.h"
#include <iostream>
#include <string>
#include <vector>

void printHelp() {
    std::cout << "Allowed options:\n"
        << "  -h, --help           produce help message\n"
        << "  -p, --port arg (=2452) set server port" << std::endl;
}

Config parseCommandLine(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "-h" || arg == "--help") {
            config.showHelp = true;
            printHelp();
            exit(0);
        }
        if (arg == "-p" || arg == "--port") {
            if (i + 1 < args.size()) {
                try {
                    config.port = static_cast<uint16_t>(std::stoul(args[++i]));
                } catch (const std::exception& e) {
                    std::cerr << "Command Line Error: Invalid port value." << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Command Line Error: --port requires an argument." << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "Command Line Error: Unknown option " << arg << std::endl;
            printHelp();
            exit(1);
        }
    }

    return config;
}
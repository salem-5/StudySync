#include "CmdParser.h"
#include <iostream>

namespace po = boost::program_options;

po::variables_map parseCommandLine(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("port,p", po::value<uint16_t>()->default_value(2452), "set server port");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        }
    } catch (const po::error& e) {
        std::cerr << "Command Line Error: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        exit(1);
    }

    return vm;
}
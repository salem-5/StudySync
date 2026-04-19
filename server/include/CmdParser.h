#pragma once
#include <map>
#include <cstdint>

struct Config {
    uint16_t port = 2452;
    uint16_t aiListenerPort = 2570;
    bool showHelp = false;
};

Config parseCommandLine(int argc, char* argv[]);
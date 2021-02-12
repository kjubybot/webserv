#include <iostream>
#include "Host.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "util.hpp"

int main(int argc, char **argv) {
    std::string confFile;
    if (argc < 2) {
        confFile = "../webserv.conf";
    } else {
        confFile = std::string(argv[1]);
    }
    Server server;
    if (!server.parseConfig(confFile))
        return 1;
    server.startServer();
}

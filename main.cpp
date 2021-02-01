#include <iostream>
#include "Server.hpp"

int main() {
    Server serv("127.0.0.1", 8080);
    serv.startServer();
}

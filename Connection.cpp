#include <cstdlib>
#include <unistd.h>
#include "Connection.hpp"

Connection::Connection(int sock) : sock(sock) {}

Connection::~Connection() {
    close(sock);
}

int Connection::getSocket() const {
    return sock;
}

void Connection::readData() {
    char *buf = (char *)malloc(4096);
    int r = read(sock, buf, 4096);
    if (r > 0)
        data.append(buf, r);
    else
        close(sock);
}

void Connection::writeData() {
    if (data.length() > 0) {
        write(sock, data.data(), data.length());
        data.clear();
    }
}
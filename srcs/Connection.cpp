#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include "Connection.hpp"
#include "util.hpp"

Connection::Connection(int sock, struct sockaddr_in sockAddr) : sock(sock), sockAddr(sockAddr), _isOpen(true) {
    std::cout << iptoa(sockAddr.sin_addr.s_addr) << std::endl;
}

Connection::~Connection() {
    close(sock);
}

int Connection::getSocket() const {
    return sock;
}

void Connection::readData() {
    char buf[4096];
    int r = read(sock, buf, 4096);
    if (r > 0)
        data.append(buf, r);
    else
        _isOpen = false;
}

void Connection::writeData() {
    if (data.length() > 0) {
        write(sock, data.data(), data.length());
        data.clear();
    }
}

bool Connection::isOpen() const {
    return _isOpen;
}

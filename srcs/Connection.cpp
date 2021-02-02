#include "Connection.hpp"

Connection::Connection(int sock, struct sockaddr_in sockAddr) : sock(sock), sockAddr(sockAddr), _isOpen(true) {
    std::cout << iptoa(sockAddr.sin_addr.s_addr) << std::endl;
}

Connection::~Connection() {
    close(sock);
}

int Connection::getSocket() const {
    return sock;
}

const Request& Connection::getRequest() const {
    return request;
}

void Connection::readData() {
    char buf[4096];
    int r = read(sock, buf, 4096);
    if (r > 0) {
        data.append(buf, r);
        std::cout << data << std::endl;
        try {
            request.parse(data);
        } catch (HttpErrorException& ex) {
            std::cout << "EXC" << std::endl;
            data = HttpErrorPage(ex.getCode(), ex.getDescription()).createPage();
            _isOpen = false;
        }
    } else
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

bool Connection::reqReady() const {
    return request.isReady();
}
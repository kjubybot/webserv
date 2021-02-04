#include "Connection.hpp"

Connection::Connection(int sock, struct sockaddr_in sockAddr) : sock(sock), sockAddr(sockAddr), _isOpen(true) {
    std::cout << "Connection from " << iptoa(sockAddr.sin_addr.s_addr) << std::endl;
}

Connection::~Connection() {
    close(sock);
}

int Connection::getSocket() const {
    return sock;
}

const Request& Connection::getRequest() const {
    return requests.front();
}

void Connection::popRequest() {
    requests.pop();
}

struct sockaddr_in Connection::getSockAddr() const {
    return sockAddr;
}

void Connection::readData() {
    char buf[4096];
    int r = read(sock, buf, 4096);
    if (r > 0) {
        data.append(buf, r);
        std::cout << data << std::endl;
        if (requests.empty() || requests.back().isReady())
            requests.push(Request());
        try {
            requests.back().parse(data);
        } catch (const HttpErrorException& ex) {
            _isOpen = false;
        }
    } else
        _isOpen = false;
}

void Connection::writeData() {
    std::string resData = responses.front().getData();
    write(sock, resData.data(), resData.length());
    responses.pop();
}

bool Connection::isOpen() const {
    return _isOpen;
}

bool Connection::reqReady() const {
    if (requests.empty())
        return false;
    return requests.front().isReady();
}

bool Connection::resReady() const {
    return !responses.empty();
}

void Connection::addResponse(const Response& r) {
    responses.push(r);
}

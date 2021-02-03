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

void Connection::readData() {
    char buf[4096];
    int r = read(sock, buf, 4096);
    if (r > 0) {
        data.append(buf, r);
        std::cout << data << std::endl;
//        if (data.find('\r') != std::string::npos) {
//            std::cout << data.substr(0, data.find('\n'));
//            if (data.substr(0, data.find('\r')) == "GET / HTTP/1.1")
//                responses.push(Response::fromString("200", "OK"));
//            return;
//        }
        if (requests.empty() || requests.back().isReady())
            requests.push(Request());
        try {
            requests.back().parse(data);
        } catch (const HttpErrorException& ex) {
            requests.pop();
            HttpErrorPage rp(ex.getCode(), ex.getDescription());
            responses.push(Response::fromString(ex.getCode(), rp.createPage()));
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
    return requests.front().isReady();
}

bool Connection::resReady() const {
    return !responses.empty();
}
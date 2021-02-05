#include "Connection.hpp"

Connection::Connection(int sock, struct sockaddr_in sockAddr, std::list<Host> hosts)
        : sock(sock), sockAddr(sockAddr), _isOpen(true), hosts(hosts) {
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
    data.append(buf, r);
    if (r > 0) {
        while (!data.empty()) {
            if (requests.empty() || requests.back().isSecondPart())
                requests.push(Request());

            if (!requests.back().isFirstPart() && data.find("\r\n\r\n") == std::string::npos)
                return;
            requests.back().parse(data);
            if (!requests.back().isSecondPart())
                return;
            if (requests.back().isFlagError()) {
                _isOpen = false;
                return;
            }
        }
    } else {
        _isOpen = false;
        if (r == 0) {
            if (requests.empty() || requests.back().isSecondPart())
                requests.push(Request());
            requests.back().parse(data);
            if (!requests.back().isSecondPart())
                requests.back().addError("400", "Bad request");
            return;
        }
    }
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
    return requests.front().isSecondPart();
}

bool Connection::resReady() const {
    return !responses.empty();
}

Host& Connection::matchHost(const Request& request) {
    std::map<std::string, std::string> headers = request.getHeaders();

    if (headers.find("host") == headers.end())
        return hosts.front();
    for (std::list<Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
        if (it->getName() == headers["host"])
            return *it;
    return hosts.front();
}

void Connection::routeRequests() {
    while (!requests.empty() && requests.front().isSecondPart()) {
        responses.push(matchHost(requests.front()).processRequest(requests.front()));
        requests.pop();
    }
}

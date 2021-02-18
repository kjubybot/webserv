#include "Connection.hpp"

Connection::Connection(int sock, struct sockaddr_in sockAddr, std::list<Host> hosts)
        : sock(sock), sockAddr(sockAddr), _isOpen(true), hosts(hosts)
{ }

Connection::~Connection() {
    close(sock);
}

int Connection::getSocket() const {
    return sock;
}

void Connection::readData() {
    char buf[IOSIZE];
    int r = read(sock, buf, IOSIZE);
    if (r > 0) {
        rdata.append(buf, r);
        while (!rdata.empty()) {
            if (requests.empty() || requests.back().isSecondPart())
                requests.push(Request(sockAddr));
            if (!requests.back().isFirstPart() && rdata.find("\r\n\r\n") == std::string::npos)
                return;
            requests.back().parse(rdata);
            if (!requests.back().isSecondPart())
                return;
            checkHeaders(requests.back());
            if (requests.back().isFlagError()) {
                _isOpen = false;
                return;
            }
        }
    } else {
        _isOpen = false;
        if (r == 0 && !rdata.empty()) {
            if (requests.empty() || requests.back().isSecondPart())
                requests.push(Request(sockAddr));
            requests.back().parse(rdata);
            if (!requests.back().isSecondPart())
                requests.back().addError("400", "Bad request");
            return;
        }
    }
}

void Connection::writeData() {
    if (!responses.empty()) {
        wdata += responses.front().getData();
        if (responses.front().isComplete())
            responses.pop();
    }
    if (!wdata.empty()) {
        wdata.erase(0, write(sock, wdata.data(), wdata.length() > IOSIZE ? IOSIZE : wdata.length()));
    }
}

bool Connection::isOpen() const {
    return _isOpen;
}

bool Connection::resReady() const {
    return (!responses.empty() || !wdata.empty());
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

void Connection::checkHeaders(Request& request) {
    Host host = matchHost(request);

    if (host.getMaxBodySize(request) != 0 && host.getMaxBodySize(request) < request.getContentLen())
        request.addError("413", "Request Entity Too Large");
    if (request.hasHeader("connection") && request.getHeader("connection") == "close")
        _isOpen = false;
}

void Connection::routeRequests() {
    while (!requests.empty() && requests.front().isSecondPart()) {
        responses.push(matchHost(requests.front()).processRequest(requests.front()));
        requests.pop();
    }
}

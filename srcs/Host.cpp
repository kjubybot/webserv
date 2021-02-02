#include <arpa/inet.h>
#include "../includes/Host.hpp"

Host::Host() {}

Host::Host(struct sockaddr_in sockAddr, const std::string& name) : sockAddr(sockAddr), name(name) {}

Host::~Host() {}

Host::Host(const Host& h) : sockAddr(h.sockAddr), name(h.name) {}

Host& Host::operator=(const Host& h) {
    sockAddr = h.sockAddr;
    name = h.name;
    return *this;
}

struct sockaddr_in Host::getSockAddr() const {
    return sockAddr;
}

bool Host::matchRequest(const Request &) const {
    return true;
}

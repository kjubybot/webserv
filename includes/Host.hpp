#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <arpa/inet.h>
#include <string>
#include "Request.hpp"

class Host {
    struct sockaddr_in sockAddr;
    std::string name;

public:
    Host();
    Host(struct sockaddr_in, const std::string& name);
    ~Host();
    Host(const Host &);
    Host &operator=(const Host &);

    struct sockaddr_in getSockAddr() const;
    bool matchRequest(const Request& r) const;
};

#endif

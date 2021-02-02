#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <list>
#include <string>
#include "Connection.hpp"

class Host {
    struct sockaddr_in sockAddr;

public:
    Host();
    Host(const std::string& address, uint16_t port);
    ~Host();
    Host(const Host &);
    Host &operator=(const Host &);

    void startServer();
};

#endif

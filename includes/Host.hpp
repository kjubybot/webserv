#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <arpa/inet.h>
#include <map>
#include <string>
#include "Request.hpp"

class Host {
    struct sockaddr_in sockAddr;
    std::string name;
    std::map<std::string, std::string> errorPages;

public:
    Host();
    Host(struct sockaddr_in, const std::string& name);
    ~Host();
    Host(const Host &);
    Host &operator=(const Host &);

    struct sockaddr_in getSockAddr() const;
    void setErrorPages(const std::map<std::string, std::string> errorPages);
    bool matchRequest(const Request& r) const;
};

#endif

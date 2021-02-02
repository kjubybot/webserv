#ifndef WEBSERV_CONNECTION_HPP
#define WEBSERV_CONNECTION_HPP

#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "Connection.hpp"
#include "HttpErrorException.hpp"
#include "HttpErrorPage.hpp"
#include "Request.hpp"
#include "util.hpp"

class Connection {
    int sock;
    struct sockaddr_in sockAddr;
    bool _isOpen;
    std::string data;
    Request request;

    Connection();
    Connection(const Connection &);
    Connection &operator=(const Connection &);
public:
    Connection(int sock, struct sockaddr_in sockAddr);
    ~Connection();

    int getSocket() const;
    const Request& getRequest() const;
    void readData();
    void writeData();
    bool isOpen() const;
    bool reqReady() const;
};


#endif

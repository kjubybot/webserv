#ifndef WEBSERV_CONNECTION_HPP
#define WEBSERV_CONNECTION_HPP

#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "Connection.hpp"
#include "HttpErrorException.hpp"
#include "HttpErrorPage.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "util.hpp"

class Connection {
    int sock;
    struct sockaddr_in sockAddr;
    bool _isOpen;
    std::string data;
    std::queue<Request> requests;
    std::queue<Response> responses;

    Connection();
    Connection(const Connection &);
    Connection &operator=(const Connection &);
public:
    Connection(int sock, struct sockaddr_in sockAddr);
    ~Connection();

    int getSocket() const;
    const Request& getRequest() const;
    struct sockaddr_in getSockAddr() const;
    void readData();
    void writeData();
    bool isOpen() const;
    bool reqReady() const;
    bool resReady() const;
    void addResponse(const Response& r);
};

#endif

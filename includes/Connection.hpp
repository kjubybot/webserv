#ifndef WEBSERV_CONNECTION_HPP
#define WEBSERV_CONNECTION_HPP

#include <cstdlib>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "Connection.hpp"
#include "HttpErrorException.hpp"
#include "HttpErrorPage.hpp"
#include "Host.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "util.hpp"

#define IOSIZE (1 << 19)

class Connection {
    int sock;
    struct sockaddr_in sockAddr;
    bool _isOpen;
    std::list<Host> hosts;
    std::string rdata;
    std::string wdata;
    std::queue<Request> requests;
    std::queue<Response> responses;

    Connection();
    Connection(const Connection &);
    Connection &operator=(const Connection &);
    Host& matchHost(const Request& request);
    void checkHeaders(Request& request);
public:
    Connection(int sock, struct sockaddr_in sockAddr, std::list<Host> hosts);
    ~Connection();

    int getSocket() const;
    const Request& getRequest() const;
    void popRequest();
    struct sockaddr_in getSockAddr() const;
    void readData();
    void writeData();
    bool isOpen() const;
    bool reqReady() const;
    bool resReady() const;
    void routeRequests();
};

#endif

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP

#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Connection.hpp"
#include "Host.hpp"
#include "Response.hpp"
#include "util.hpp"

class Server {
    std::list<int> sockets;
    std::list<Connection*> connections;
    std::list<Host> hosts;

    Server(const Server &);
    Server &operator=(const Server &);
    Host parseHost(int fd);
    Host matchHost(const Connection& conn, const Request& r);
    bool makeSockets();
public:
    Server();
    ~Server();

    bool parseConfig(const std::string& filename);
    void startServer();
};

#endif

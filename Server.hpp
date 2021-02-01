#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP

#include <list>
#include <string>
#include "Connection.hpp"

class Server {
    std::string address;
    uint16_t port;
    int listenSock;
    std::list<Connection*> connections;

    Server();
    Server(const Server &);
    Server &operator=(const Server &);
public:
    Server(const std::string& address, uint16_t port);
    ~Server();


    void startServer();
};

#endif

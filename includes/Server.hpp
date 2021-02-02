#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP

#include <list>
#include "Connection.hpp"
#include "Host.hpp"

class Server {
    std::list<int> sockets;
    std::list<Connection*> connections;
    std::list<Host> hosts;

    Server(const Server &);
    Server &operator=(const Server &);
    Host parseHost(int fd);
    bool makeSockets();
public:
    Server();
    ~Server();

    bool parseConfig(const std::string& filename);
    void startServer();
};


#endif

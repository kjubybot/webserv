#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Server.hpp"

Server::Server(const std::string &address, uint16_t port) : address(address), port(port), listenSock(0) {}

Server::~Server() {
    for (std::list<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it)
        delete *it;
    close(listenSock);
}

void Server::startServer() {
    struct sockaddr_in listenAddr;
    fd_set rfds, wfds;

    if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Unable to create socket" << std::endl;
        return;
    }
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = (port >> 8) | (port << 8);
    listenAddr.sin_addr.s_addr = inet_addr(address.c_str());
    if (bind(listenSock, (struct sockaddr *) &listenAddr, sizeof(listenAddr))) {
        std::cerr << "Unable to bind address: " << strerror(errno) << std::endl;
        close(listenSock);
        return;
    }
    if (listen(listenSock, -1)) {
        std::cerr << "Unable to enter listen mode: " << strerror(errno) << std::endl;
        close(listenSock);
        return;
    }
    while (true) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(listenSock, &rfds);

        for (std::list<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
            if ((*it)->isOpen()) {
                FD_SET((*it)->getSocket(), &rfds);
                FD_SET((*it)->getSocket(), &wfds);
            } else {
                delete *it;
                connections.erase(it);
            }
        }
        if (select(100, &rfds, &wfds, 0, 0) > 0) {
            if (FD_ISSET(listenSock, &rfds)) {
                int newSock = accept(listenSock, 0, 0);
                if (newSock < 0)
                    std::cerr << "Unable too create connection: " << strerror(errno) << std::endl;
                else
                    connections.push_back(new Connection(newSock));
            }
            for (std::list<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
                if (FD_ISSET((*it)->getSocket(), &rfds))
                    (*it)->readData();
                if (FD_ISSET((*it)->getSocket(), &wfds))
                    (*it)->writeData();
            }
        }
        usleep(1000);
    }
}
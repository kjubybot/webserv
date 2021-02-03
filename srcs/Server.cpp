#include "Server.hpp"

Server::Server() {}

Server::~Server() {
    for (std::list<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
        close((*it)->getSocket());
        delete *it;
    }
}

bool Server::parseConfig(const std::string &filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    std::string line;
    if (fd < 0) {
        std::cerr << "Unable to read config: " << strerror(errno) << std::endl;
        return false;
    }
    while (get_next_line(fd, line)) {
        line = trim(line);
        if (line == "ServerStart") {
            try {
                hosts.push_back(parseHost(fd));
            } catch (const char* err) {
                std::cerr << "Unable to parse config: " << err << std::endl;
                return false;
            }
        }
    }
    close(fd);
    return true;
}

Host Server::parseHost(int fd) {
    std::string line, name;
    std::vector<std::string> items;
    struct sockaddr_in sockAddr;

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = 0;
    while (get_next_line(fd, line)) {
        if (line == "ServerEnd")
            break;
        items = split(line, " ");
        if (items[0] == "name") {
            if (items.size() < 2)
                throw "incomplete name directive";
            if (!name.empty())
                throw "duplicate name directive";
            name = items[1];
        } else if (items[0] == "listen") {
            if (items.size() < 2)
                throw "incomplete listen directive";
            items = split(items[1], ":");
            if (items.size() < 2) {
                if (items[0].find('.') != std::string::npos)
                    throw "missing port in listen directive";
                sockAddr.sin_addr.s_addr = 0;
                sockAddr.sin_port = std::stoi(items[0]);
            } else {
                sockAddr.sin_addr.s_addr = inet_addr(items[0].c_str());
                sockAddr.sin_port = std::stoi(items[1]);
            }
        }
    }
    sockAddr.sin_port = (sockAddr.sin_port >> 8) | (sockAddr.sin_port << 8);
    return Host(sockAddr, name);
}

bool Server::makeSockets() {
    std::list<struct sockaddr_in> sockAddrs;
    std::list<struct sockaddr_in>::iterator i, j;
    int sock;

    for (std::list<Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
        sockAddrs.push_back(it->getSockAddr());

    // it is a monster, removing duplicate socket addresses
    i = sockAddrs.begin();
    while (i != sockAddrs.end()) {
        j = ++i;
        --i;
        while (j != sockAddrs.end()) {
            if (i->sin_port == j->sin_port) {
                if (i->sin_addr.s_addr == j->sin_addr.s_addr)
                    sockAddrs.erase(j--);
                else if (i->sin_addr.s_addr == 0)
                    sockAddrs.erase(j--);
                else if (j->sin_addr.s_addr == 0) {
                    sockAddrs.erase(i--);
                    break;
                }
            }
            ++j;
        }
        ++i;
    }
    i = sockAddrs.begin();
    while (i != sockAddrs.end()) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Unable to create socket: " << strerror(errno) << std::endl;
            return false;
        }
        sockets.push_back(sock);
        if (bind(sock, (struct sockaddr *)&(*i), sizeof(*i))) {
            std::cerr << "Unable to bind socket: " << strerror(errno) << std::endl;
            return false;
        }
        if (listen(sock, 50)) {
            std::cerr << "Unable to enter listen mode: " << strerror(errno) << std::endl;
            return false;
        }
        ++i;
    }
    return true;
}

void Server::routeRequest(Connection &conn) {
    const Request req = conn.getRequest();

    for (std::list<Host>::iterator it = hosts.begin(); it != hosts.end(); ++it)
        if (it->matchRequest(req))
            return;
}

void Server::startServer() {
    struct sockaddr_in sockAddr;
    socklen_t sockLen;
    fd_set rfds, wfds;

    if (!makeSockets())
        return;
    std::cout << "|" << trim("     Starting server      ") << "|" << std::endl;
    while (true) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        for (std::list<int>::iterator it = sockets.begin(); it != sockets.end(); ++it)
            FD_SET(*it, &rfds);
        for (std::list<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
            if ((*it)->resReady())
                FD_SET((*it)->getSocket(), &wfds);
            if ((*it)->isOpen()) {
                FD_SET((*it)->getSocket(), &rfds);
            } else if (!(*it)->resReady()){
                delete *it;
                connections.erase(it);
            }
        }
        if (select(100, &rfds, &wfds, 0, 0) > 0) {
            for (std::list<int>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
                if (FD_ISSET(*it, &rfds)) {
                    int newSock = accept(*it, (struct sockaddr *) &sockAddr, &sockLen);
                    if (newSock < 0)
                        std::cerr << "Unable too create connection: " << strerror(errno) << std::endl;
                    else
                        connections.push_back(new Connection(newSock, sockAddr));
                }
            }
            for (std::list<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
                if (FD_ISSET((*it)->getSocket(), &rfds))
                    (*it)->readData();
//                if ((*it)->reqReady())
//                    routeRequest(*it);
                if (FD_ISSET((*it)->getSocket(), &wfds))
                    (*it)->writeData();
            }
        }
        usleep(1000);
    }
}

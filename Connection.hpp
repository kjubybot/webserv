#ifndef WEBSERV_CONNECTION_HPP
#define WEBSERV_CONNECTION_HPP

#include <string>

class Connection {
    int sock;
    std::string data;

    Connection();
    Connection(const Connection &);
    Connection &operator=(const Connection &);
public:
    Connection(int sock);
    ~Connection();

    int getSocket() const;
    void readData();
    void writeData();
};


#endif

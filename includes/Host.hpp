#ifndef WEBSERV_HOST_HPP
#define WEBSERV_HOST_HPP

#include <arpa/inet.h>
#include <dirent.h>
#include <map>
#include <string>
#include <sys/stat.h>
#include "HttpErrorPage.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Host {
    struct sockaddr_in sockAddr;
    std::string name;
    std::string root;
    bool autoindex;
    std::map<std::string, std::string> errorPages;

    std::string makeAutoindex(const std::string& path) const;
    Response makeError(const std::string& code, const std::string& message);
public:
    Host();
    Host(struct sockaddr_in, const std::string& name);
    ~Host();
    Host(const Host &);
    Host &operator=(const Host &);

    struct sockaddr_in getSockAddr() const;
    const std::string& getName() const;
    const std::string& getRoot() const;
    void setRoot(const std::string& root);
    void setErrorPages(const std::map<std::string, std::string> errorPages);
    Response processRequest(const Request& r) const;
};

#endif

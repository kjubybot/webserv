#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include <map>
#include <string>

class Response {
    std::string code;
    std::string body;
    std::map<std::string, std::string> n;
public:
    Response();
    ~Response();
    Response(const Response &);
    Response &operator=(const Response &);
};

#endif

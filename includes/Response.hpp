#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include <fcntl.h>
#include <map>
#include <string>
#include <unistd.h>

class Response {
    std::string code;
    std::string body;
    std::map<std::string, std::string> headers;


public:
    Response();
    Response(const std::string& code);
    Response(const std::string& code, const std::string& body);
    Response(const Response& r);
    Response &operator=(const Response& r);
    ~Response();

    static Response fromFile(const std::string& code, const std::string& filename);
    static Response fromString(const std::string& code, const std::string& body);
    std::string getData() const;
};

#endif

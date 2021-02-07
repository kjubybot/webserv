#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include <fcntl.h>
#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

class Response {
    std::string code;
    std::string message;
    std::string body;
    std::map<std::string, std::string> headers;

    static std::string getDate();
    static std::string getLastModified(const std::string& filename);
    static std::string getContentType(const std::string& filename);
public:
    Response();
    Response(const std::string& code);
    Response(const std::string& code, const std::string& body);
    Response(const Response& r);
    Response &operator=(const Response& r);
    ~Response();

    static Response fromFile(const std::string& code, const std::string& message, const std::string& filename);
    static Response fromFileNoBody(const std::string& code, const std::string& message, const std::string& filename);
    static Response fromString(const std::string& code, const std::string& message, const std::string& body);
    std::string getData();
    void setCode(const std::string& code);
    void setMessage(const std::string& message);
    void setBody(const std::string& body);
    void setHeader(const std::string& key, const std::string& value);
};

#endif
